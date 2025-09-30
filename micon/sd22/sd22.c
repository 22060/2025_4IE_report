#include <7080S.H>
#include "libmemes.h"

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

#define SD_CD (PE.DR.BIT.B11)
#define SD_CS (PE.DR.BIT.B9)

// ------------------------------------------------------------
void init_CMT0()
{
    STB.CR4.BIT._CMT = 0;

    CMT0.CMCNT = 0;
    CMT0.CMCSR.BIT.CKS = 1; // CKS設定(0:1/8, 1:1/32, 2:1/128, 3:1/512)
}

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

// ------------------------------------------------------------
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

// ------------------------------------------------------------
int card_exist()
{
    return (PE.DR.BIT.B11 ? 0 : 1);
}

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

// ------------------------------------------------------------
void main()
{
    int i, j;
    unsigned char ret;
    unsigned int SECT_NR;
    unsigned char ch, dt[512];

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

            // -- 演習２ --
            printf("CMD17\n");
            // 手順１：CMD17 を発行し、データ・トークン・スタートバイトを待つ
            // CMD17 の引数は 0
            ret = SD_send_cmd(0x51, 0); // CMD17 with argument 0
            while (ret != 0xfe)         // Wait for data token start byte
            {
                ret = SPI_tx_rx(0xff);
            }

            // 手順２：512 バイト分のデータを取得し、配列に格納
            for (i = 0; i < 512; i++)
            {
                dt[i] = SPI_tx_rx(0xff);
                // printf("0x%02x ", dt[i]);
            }
            // printf("\n");

            // 手順３：取得したデータを表示
            printf("1st block\n");
            for (i = 0; i < 512; i++)
            {
                if (i % 16 == 0 && i != 0)
                    printf("\n");
                printf("%02x ", dt[i]);
            }
            printf("\n");

            // -- 演習３ --
            // 第一パーティションのセクタ番号(SECT_NR)計算、表示
            // dt[454]~dt[457]で，リトルエンディアンで格納されている
            SECT_NR = dt[454] | (dt[455] << 8) | (dt[456] << 16) | (dt[457] << 24);
            printf("1st partition sector number: %x\n", SECT_NR);

            // CMD17 を発行し、データ・トークン・スタートバイトを待つ
            // CMD17 の引数は SECT_NR * 512
            ret = SD_send_cmd(0x51, SECT_NR * 512); // CMD17 with argument SECT_NR * 512
            while (ret != 0xfe)                     // Wait for data token start byte
            {
                ret = SPI_tx_rx(0xff);
            }

            // 512 バイト分のデータを取得し、配列に格納
            for (i = 0; i < 512; i++)
            {
                dt[i] = SPI_tx_rx(0xff);
            }

            // 取得したデータを表示
            printf("1st partition\n");
            for (i = 0; i < 512; i++)
            {
                if (i % 16 == 0 && i != 0)
                    printf("\n");
                printf("%02x ", dt[i]);
            }
            printf("\n");
        }
    }
    SD_CS = 1; // CS negate

    while (1)
        ;
}
