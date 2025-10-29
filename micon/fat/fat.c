#include <7080S.H>
#include "libmemes.h"

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

#define SD_CD (PE.DR.BIT.B11)
#define SD_CS (PE.DR.BIT.B9)

// --------------------------------------------------
struct disk_t
{
    unsigned int First_sect_LBA;
    unsigned int BPB_BytesPerSec;
    unsigned int BPB_SecPerClus;
    unsigned int BPB_RsvdSecCnt;
    unsigned int BPB_NumFATs;
    unsigned int BPB_RootEntCnt;
    unsigned int BPB_FATSz16;
    unsigned int First_RDE_sect;
    unsigned int First_FAT_sect;
    unsigned int First_Data_sect;
} DISK;

struct file_t
{
    unsigned char Filename[12];
    unsigned int n;          // RDE の(ゼロから数えて) n 番目のファイル
    unsigned int attr;       // 属性
    unsigned int File_year;  // 年
    unsigned int File_month; // 月
    unsigned int File_date;  // 日
    unsigned int File_hour;  // 時
    unsigned int File_min;   // 分
    unsigned int File_sec;   // 秒
    unsigned int FstClusLO;
    unsigned int FileSize;
    unsigned char *Data; // ファイル本体
};

// --------------------------------------------------
// -- グローバル変数 --
unsigned char dt[512];   // セクタリード用ワーク
unsigned char fat[1024]; // FAT
unsigned char rde[512];  // RDE

struct file_t File0;           // ファイル
unsigned char FileData0[4096]; // ファイルデータ本体

// --------------------------------------------------
// --------------------------------------------------
void init_CMT0()
{
    STB.CR4.BIT._CMT = 0;

    CMT0.CMCNT = 0;
    CMT0.CMCSR.BIT.CKS = 1; // CKS設定(0:1/8, 1:1/32, 2:1/128, 3:1/512)
}

// --------------------------------------------------
void wait_us(unsigned int us)
{
    unsigned int val;

    val = us * 10 / 16;
    if (val >= 0xffff)
        val = 0xffff;

    CMT0.CMCOR = val;
    CMT0.CMCSR.BIT.CMF &= 0;
    CMT.CMSTR.BIT.STR0 = 1;

    while (CMT0.CMCSR.BIT.CMF == 0)
        ;
    CMT0.CMCSR.BIT.CMF = 0;
    CMT.CMSTR.BIT.STR0 = 0;
}

// --------------------------------------------------
void init_SCI2()
{
    STB.CR3.BIT._SCI2 = 0;
    SCI2.SCSCR.BYTE = 0x00;
    SCI2.SCSMR.BIT.CA = 1;   // クロック同期式
    SCI2.SCBRR = 12;         // 384kbps
    SCI2.SCSDCR.BIT.DIR = 1; // MSB first
    wait_us(1);
    PFC.PECRL3.BIT.PE10MD = 2; // PE10 .. TxD
    PFC.PECRL3.BIT.PE8MD = 2;  // PE8 .. SCK
    PFC.PECRL2.BIT.PE7MD = 2;  // PE7 .. RxD
    PFC.PEIORL.BIT.B12 = 0;    // PE12 .. WP入力
    PFC.PEIORL.BIT.B11 = 0;    // PE11 .. CD入力
    PFC.PEIORL.BIT.B9 = 1;     // PE9 .. CS出力
    PE.DR.BIT.B9 = 1;          // CS初期値
    SCI2.SCSCR.BYTE |= 0x30;   // TE, RE = 1
}

// --------------------------------------------------
unsigned char SPI_tx_rx(unsigned char ch)
{
    while (!SCI2.SCSSR.BIT.TDRE)
        ;
    SCI2.SCTDR = ch;
    SCI2.SCSSR.BIT.TDRE = 0;

    while (!SCI2.SCSSR.BIT.RDRF)
        ;
    ch = SCI2.SCRDR;
    SCI2.SCSSR.BIT.RDRF = 0;
    return (ch);
}

// --------------------------------------------------
int card_exist()
{
    return (PE.DR.BIT.B11 ? 0 : 1);
}

// --------------------------------------------------
unsigned char calc_CRC7(unsigned char *data, int len)
{
    int i, j;
    char crc, dt;

    crc = 0;
    for (i = 0; i < len; i++)
    {
        dt = *data++;
        for (j = 0; j < 8; j++)
        {
            crc <<= 1;
            if ((crc & 0x80) ^ (dt & 0x80))
                crc ^= 0x09;
            dt <<= 1;
        }
    }
    return (crc & 0x7f);
}

// --------------------------------------------------
unsigned char SD_send_cmd(unsigned char cmd, int arg)
{
    int i;
    unsigned char cmd_token[6], ret;

    while (1)
    {
        if (SPI_tx_rx(0xff) == 0xff)
            break;
    }

    cmd_token[0] = cmd;
    cmd_token[1] = (arg >> 24) & 0xff;
    cmd_token[2] = (arg >> 16) & 0xff;
    cmd_token[3] = (arg >> 8) & 0xff;
    cmd_token[4] = arg & 0xff;
    cmd_token[5] = (calc_CRC7(cmd_token, 5) << 1) | 0x01;

    for (i = 0; i < 6; i++)
        SPI_tx_rx(cmd_token[i]);

    do
    {
        ret = SPI_tx_rx(0xff);
    } while (ret & 0x80);

    return (ret);
}

// --------------------------------------------------
int Enter_SPI_mode()
{
    int i;
    unsigned char ret;

    SD_CS = 1; // CS negate

    for (i = 0; i < 10; i++)
        SPI_tx_rx(0xff);

    SD_CS = 0; // CS assert

    ret = SD_send_cmd(0x40, 0);
    if (ret != 0x01)
        return (-1);

    do
    {
        ret = SD_send_cmd(0x41, 0);
    } while (ret != 0x00);

    return (1);
}

// --------------------------------------------------
void read_sector(unsigned int sect, unsigned char *dt)
{
    unsigned char ch;
    int i;

    ch = SD_send_cmd(0x51, sect * 512);
    while (ch != 0xfe)
        ch = SPI_tx_rx(0xff);

    for (i = 0; i < 512; i++)
        *dt++ = SPI_tx_rx(0xff);
    SPI_tx_rx(0xff); // CRC分
    SPI_tx_rx(0xff); // CRC分
}

// --------------------------------------------------
void print_sector(unsigned int sect, unsigned char *dt)
{
    int i, j;
    unsigned int adr;

    adr = sect * 512;

    printf("           ");
    for (i = 0; i < 16; i++)
    {
        printf("+%x ", i);
        if (i == 7)
            printf(" ");
    }
    printf("\n");

    for (i = 0; i < 32; i++)
    {
        printf("%08x : ", adr);
        for (j = 0; j < 16; j++)
        {
            printf("%02x ", dt[i * 16 + j]);
            if (j == 7)
                printf(" ");
        }
        printf(" ");
        for (j = 0; j < 16; j++)
        {
            if (isprint(dt[i * 16 + j]))
                printf("%c", dt[i * 16 + j]);
            else
                printf(".");
            if (j == 7)
                printf(" ");
        }
        printf("\n");
        adr += 16;
    }
}

// --------------------------------------------------
// --------------------------------------------------
// MBR を解析する
//
void ex_MBR()
{
    // -- 演習１ --
    read_sector(0, dt);
    // printf("dt[454]=0x%02x dt[455]=0x%02x dt[456]=0x%02x dt[457]=0x%02x\n", dt[454], dt[455], dt[456], dt[457]);
    DISK.First_sect_LBA = dt[454] | (dt[455] << 8) | (dt[456] << 16) | (dt[457] << 24);
}

// --------------------------------------------------
// BPB を解析する
//
void ex_BPB()
{
    // -- 演習２ --
    read_sector(DISK.First_sect_LBA, dt);
    DISK.BPB_BytesPerSec = dt[11] | (dt[12] << 8);
    DISK.BPB_SecPerClus = dt[13];
    DISK.BPB_RsvdSecCnt = dt[14] | (dt[15] << 8);
    DISK.BPB_NumFATs = dt[16];
    DISK.BPB_RootEntCnt = dt[17] | (dt[18] << 8);
    DISK.BPB_FATSz16 = dt[22] | (dt[23] << 8);
}

// --------------------------------------------------
// RDE を取得
//
void get_RDE(int n)
{
    int i;
    // -- 演習３ --
    DISK.First_RDE_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt + (DISK.BPB_NumFATs * DISK.BPB_FATSz16);
    read_sector(DISK.First_RDE_sect + n, rde);
    // print_sector(DISK.First_RDE_sect + n, rde);
}

// --------------------------------------------------
// RDE の n 番目のファイル情報を取得
//
int get_file_info(struct file_t *file)
{
    // -- 演習４ --
    int i;
    unsigned int offset;

    offset = file->n * 32;
    if (rde[offset] == 0xe5 && rde[offset + 1] == 0x4a)
    { // 削除ファイル
        return (-1);
    }
    if (rde[offset] == 0x00)
    { // 空きエントリ
        file->Filename[0] = 0x00;
        return (0);
    }

    // -- ファイル名 --
    for (i = 0; i < 8; i++)
        file->Filename[i] = rde[offset + i];

    file->Filename[8] = '.';

    // -- 拡張子 --
    for (i = 8; i < 11; i++)
        file->Filename[i + 1] = rde[offset + i];

    // -- ここから編集 --
    file->attr = rde[offset + 11];
    file->File_hour = (rde[offset + 15] >> 3) & 0x1f;
    file->File_min = ((rde[offset + 15] & 0x07) << 3) + (rde[offset + 14] >> 5);
    file->File_sec = (rde[offset + 14] & 0x1f) * 2;
    file->File_year = ((rde[offset + 17] >> 1) & 0x7f) + 1980;
    file->File_month = ((rde[offset + 16] >> 5) & 0x07) + (((rde[offset + 17]) & 0x01) * 8);
    file->File_date = rde[offset + 16] & 0x1f;
    file->FstClusLO = rde[offset + 26] | (rde[offset + 27] << 8);
    file->FileSize = rde[offset + 28] | (rde[offset + 29] << 8) | (rde[offset + 30] << 16) | (rde[offset + 31] << 24);
    return (1);
}

// --------------------------------------------------
// FAT を取得
// サイズが大きいファイルに対応するため、FAT は 2 セクタ分取得しておく
//
void get_FAT()
{
    int i;
    // -- 演習５ --
    DISK.First_FAT_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt;
    read_sector(DISK.First_FAT_sect, fat);
    read_sector(DISK.First_FAT_sect + 1, &fat[512]);
    for (i = 0; i < 32; i++)
        printf("%02x ", fat[i]);
    printf("\n");
}

// --------------------------------------------------
// クラスタチェーンを表示
//
void ex_FAT(struct file_t *file)
{
    // -- 演習６ --
    unsigned int cluster;

    cluster = file->FstClusLO;
    printf("%d ", cluster);
    while (cluster != 0xffff)
    {
        cluster = fat[cluster * 2] | (fat[cluster * 2 + 1] << 8);
        printf(" -> %d ", cluster);
    }
    printf("\n");
}

// --------------------------------------------------
// ファイルを取得
//
void get_File(struct file_t *file)
{
    // -- 演習７ --
    int i, j;
    unsigned int sz;
    unsigned int cluster;

    DISK.First_Data_sect = DISK.First_sect_LBA + DISK.BPB_RsvdSecCnt + (DISK.BPB_NumFATs * DISK.BPB_FATSz16) + ((DISK.BPB_RootEntCnt * 32 + (DISK.BPB_BytesPerSec - 1)) / DISK.BPB_BytesPerSec);
    cluster = file->FstClusLO;
    // printf("First_Data_sect=0x%x\n", (DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus));
    sz = 0;
    while (cluster != 0xffff)
    {
        for (i = 0; i < DISK.BPB_SecPerClus; i++)
        {
            read_sector(DISK.First_Data_sect + (cluster - 2) * DISK.BPB_SecPerClus + i, dt);
            for (j = 0; j < 512; j++)
            {
                // printf("%c ", dt[j]);
                file->Data[sz++] = dt[j];
                if (sz >= file->FileSize)
                {
                    break;
                }
            }
            if (sz >= file->FileSize)
                break;
        }
        if (sz >= file->FileSize)
            break;
        cluster = fat[cluster * 2] | (fat[cluster * 2 + 1] << 8);
        // printf("Next cluster=%d\n", cluster);
    }
}

// --------------------------------------------------
// ファイル内容を表示
//
void print_File(struct file_t *file)
{
    int i;

    for (i = 0; i < file->FileSize; i++)
        printf("%c", file->Data[i]);
    printf("\n");
}

// --------------------------------------------------
// セクタダンプ
//
void dump_SECT()
{
    unsigned int s;
    unsigned int n;
    int i = 0;
    for (i = 0; i < 4096; i++)
        File0.Data[i] = 0;

    printf("\nfile shower(s,n) = ");
    scanf("%d", &s);
    scanf("%d", &n);
    get_RDE(s);
    File0.n = n; // RDE 3 番目のファイル情報
    File0.Data = FileData0;
    if (get_file_info(&File0) == 1)
    {
        for (i = 0; i < 12; i++)
            printf("%c", File0.Filename[i]);
        printf(" -> ");

        get_File(&File0);
        print_File(&File0);
    }
    else if (get_file_info(&File0) == -1)
    {
        printf("Deleted file\n");
    }
    else
    {
        printf("No file\n");
    }
}
// --------------------------------------------------
// show file information
void show_file_info(struct file_t *file)
{
    int i = 0;
    int status;
    printf("------------------------------------------------\n");
    while (status = get_file_info(file))
    {
        printf("s=%X ", i);
        printf("n=%d ", file->n);
        if (status == -1)
        {
            printf("削除ファイル");
            file->n++;
        }
        else
        {
            for (int i = 0; i < 12; i++)
                printf("%c", file->Filename[i]);
            printf(" / ");
            // printf("属性 = 0x%x\n", file->attr);
            printf("Time = %02d:%02d:%02d / ", file->File_hour, file->File_min, file->File_sec);
            printf("Date = %4d/%02d/%02d", file->File_year, file->File_month, file->File_date);
            // printf("FileSize = %d\n", file->FileSize);
            // printf("FstClusLO = %d\n", file->FstClusLO);
            file->n++;
        }
        if (file->n > (DISK.BPB_RootEntCnt / 32) - 1)
        {
            get_RDE(++i);
            file->n = 0;
        }
        printf("\n");
        ex_FAT(file);
    }
}
// --------------------------------------------------
// --------------------------------------------------
void main()
{
    int i;
    int j;

    init_CMT0();
    init_SCI2();

    if (0)
        printf("No card found\n");
    else
    {
        if (Enter_SPI_mode() < 0)
            printf("SPI mode Err\n");
        else
        {
            printf("SPI mode\n");

            SD_send_cmd(0x50, 512); // ブロックサイズ=512

            // --------------------------------------------------
            // MBR を解析 .. First_sect_LBA を取得
            //			ex_MBR();
            //			printf("1st_sector(LBA) = 0x%x\n\n", DISK.First_sect_LBA);
            ex_MBR();
            printf("1st_sector(LBA) = 0x%x\n\n", DISK.First_sect_LBA);

            // --------------------------------------------------
            // BPB を解析 .. BPB_RsvdSecCnt 等を取得
            ex_BPB();
            printf("BPB_BytesPerSec = %d\n", DISK.BPB_BytesPerSec);
            printf("BPB_SecPerClus = %d\n", DISK.BPB_SecPerClus);
            printf("BPB_RsvdSecCnt = %d\n", DISK.BPB_RsvdSecCnt);
            printf("BPB_NumFATs = %d\n", DISK.BPB_NumFATs);
            printf("BPB_RootEntCnt = %d\n", DISK.BPB_RootEntCnt);
            printf("BPB_FATSz16 = %d\n\n", DISK.BPB_FATSz16);

            // --------------------------------------------------
            // FAT を取得 -> グローバル変数 fat[] に格納
            get_FAT();

            // --------------------------------------------------
            // RDE を取得 -> グローバル変数 rde[] に格納
            get_RDE(0);
            printf("First RDE sect = 0x%x\n", DISK.First_RDE_sect);

            // --------------------------------------------------
            // RDE よりファイル情報を取得
            File0.n = 3; // RDE 3 番目のファイル情報
            File0.Data = FileData0;
            get_file_info(&File0);
            for (i = 0; i < 12; i++)
                printf("%c", File0.Filename[i]);
            printf("\n");
            printf("属性 = 0x%x\n", File0.attr);
            printf("Time = %02d:%02d:%02d\n", File0.File_hour, File0.File_min, File0.File_sec);
            printf("Date = %4d/%02d/%02d\n", File0.File_year, File0.File_month, File0.File_date);
            printf("FileSize = %d\n", File0.FileSize);
            printf("FstClusLO = %d\n", File0.FstClusLO);

            // --------------------------------------------------
            // File に対応する FAT を解析し、クラスタチェーンを表示
            ex_FAT(&File0);

            // --------------------------------------------------
            // File に対応するファイル本体を取得 -> File.data[] に格納
            get_File(&File0);

            // --------------------------------------------------
            // ファイル内容を表示
            printf("FileSize = %d\n", File0.FileSize);
            print_File(&File0);

            // --------------------------------------------------
            // ルートディレクトリのファイル情報をすべて表示
            File0.n = 3;
            show_file_info(&File0);

            // --------------------------------------------------
            while (1)
            {
                dump_SECT(); // セクタダンプ
            }
        }
    }

    SD_CS = 1; // CS negate

    while (1)
        ;
}
