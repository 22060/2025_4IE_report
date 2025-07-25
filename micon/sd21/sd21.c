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
    int i;
    unsigned char ch, dt[20];
    unsigned int C_SIZE, C_SIZE_MULT, READ_BL_LEN, CARD_SIZE;

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

            // -- ここに追加 --
            // 手順１：コマンドを発行し、CSD レジスタを取得・表示
            SD_send_cmd(0x49, 0); // CMD9 .. CSD レジスタ取得
            for (i = 0; i < 16; i++)
            {
                dt[i] = SPI_tx_rx(0xff);
                printf("0x%02x ", dt[i]);
            }
            printf("\n");

            // 手順２：READ_BL_LEN を計算
            READ_BL_LEN = dt[6] & 0xf; // <-- 変更
            printf("READ_BL_LEN = %d\n", READ_BL_LEN);

            // 手順３：C_SIZE を計算
            C_SIZE = 0;                    // <-- 変更
            C_SIZE = (dt[7] & 0x03) << 10; // C_SIZEの上位2ビット
            C_SIZE |= dt[8] << 2;          // C_SIZEの中間8ビット
            C_SIZE |= (dt[9] & 0xc0) >> 6; // C_SIZEの下位6ビット
            printf("C_SIZE = %d\n", C_SIZE);
            C_SIZE += 1;

            // 手順４：C_SIZE_MULT を計算
            C_SIZE_MULT = 0;                     // <-- 変更
            C_SIZE_MULT = (dt[10] & 0x03) << 1;  // C_SIZE_MULTの上位2ビット
            C_SIZE_MULT |= (dt[11] & 0x80) >> 7; // C_SIZE_MULTの下位1ビット
            printf("C_SIZE_MULT = %d\n", C_SIZE_MULT);
            C_SIZE_MULT += 2;

            // 手順５：CARD_SIZE を計算
            CARD_SIZE = 0; // <-- 変更
            CARD_SIZE = C_SIZE * (1 << C_SIZE_MULT) * (1 << READ_BL_LEN);
            CARD_SIZE /= 1024; // KB単位に変換
            printf("CARD_SIZE = %d KB\n", CARD_SIZE);
            // -- ここまで --
        }
    }
    SD_CS = 1; // CS negate

    while (1)
        ;
}
