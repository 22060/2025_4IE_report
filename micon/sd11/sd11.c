#include "libmemes.h"
#include <7080S.H>

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
    SCI2.SCBRR = 0x04;       // 1Mbps
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
    unsigned char data;
    // -- 演習１ --
    while ((SCI2.SCSSR.BIT.TDRE == 0))
        ;            // 送信可能になるまで待つ
    SCI2.SCTDR = ch; // 送信データをセット
    SCI2.SCSSR.BIT.TDRE = 0;
    while ((SCI2.SCSSR.BIT.RDRF == 0))
        ;                    // 受信データがあるまで待つ
    data = SCI2.SCRDR;       // 受信データを返す
    SCI2.SCSSR.BIT.RDRF = 0; // 受信フラグをクリア
    return data;
}

// ------------------------------------------------------------
int card_exist()
{
    return 1;
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

void SD_send_cmd(unsigned char cmd, int arg)
{
    int i;
    unsigned char cmd_token[6];

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
    {
        SPI_tx_rx(cmd_token[i]);
        printf("0x%02x ", cmd_token[i]);
    }
    printf("\n");
}

// ------------------------------------------------------------
void main()
{
    int i;
    unsigned char ret;

    init_CMT0();
    init_SCI2();

    if (!card_exist())
        printf("No card found\n");
    else
    {
        // ダミークロック80個発行
        for (i = 0; i < 10; i++)
            SPI_tx_rx(0xff);

        SD_CS = 0; // CS assert

        SD_send_cmd(0x40, 0); // CMD0発行
        do
        {
            ret = SPI_tx_rx(0xff);
            printf("0x%02x\n", ret);
        } while (ret & 0x80);
    }
    SD_CS = 1; // CS negate

    while (1)
        ;
}
