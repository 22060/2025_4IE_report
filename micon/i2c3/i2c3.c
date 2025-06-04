/*
 * i2c22.c
 */

#include <7080S.H>
#include "libmemes.h"

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

// ------------------------------------------------------------
#define ACK 1
#define NAK -1

// ------------------------------------------------------------
unsigned short font_data[95][32];

// ------------------------------------------------------------
// 開始条件(start condition)を発行する
void I2C_start(void)
{
    IIC2.ICCR2.BYTE = (IIC2.ICCR2.BYTE & 0xbf) | 0x80;
    while (!IIC2.ICSR.BIT.TDRE)
        ;
}

// 停止条件(stop condition)を発行する
void I2C_stop(void)
{
    IIC2.ICSR.BIT.STOP = 0;
    IIC2.ICCR2.BYTE &= 0x3f;
    while (!IIC2.ICSR.BIT.STOP)
        ;
}

char I2C_tx_byte(char data)
{
    IIC2.ICDRT = data;
    while (!IIC2.ICSR.BIT.TDRE)
        ;
    if (IIC2.ICIER.BIT.ACKBR)
        return (NAK);
    else
        return (ACK);
}

// ------------------------------------------------------------
// EEPROMへのバイト書き込み
void I2C_EEwrite_byte(unsigned short adr, unsigned char data)
{
    char ack, ret, tmp;

    while (IIC2.ICCR2.BIT.BBSY)
        // BBSY == 1の間
        ;

    // マスタ送信モードに設定
    IIC2.ICCR1.BIT.MST = 1; // マスタモード
    IIC2.ICCR1.BIT.TRS = 1; // 送信モード

    do
    {
        I2C_start();       // 開始条件を発行
        I2C_tx_byte(0xa0); // Control byteを送信
        while (!IIC2.ICSR.BIT.TEND)
            ;
        ack = IIC2.ICIER.BIT.ACKBR;
    } while (ack == 1);
    I2C_tx_byte((adr >> 8) & 0xff); // アドレス上位
    I2C_tx_byte(adr & 0xff);        // アドレス下位
    I2C_tx_byte(data);              // 書き込みデータ
    while (!IIC2.ICSR.BIT.TEND)
        ;

    I2C_stop(); // 停止条件発行

    // スレーブ受信モードに戻しておく
    IIC2.ICCR1.BIT.MST = 0; // スレーブモード
    IIC2.ICCR1.BIT.TRS = 0; // 受信モード

    //	IIC2.ICSR.BIT.TEND = 0;			////////////
    IIC2.ICSR.BIT.TDRE = 0; ////////////
}

// EEPROMから１バイト読み出す
unsigned char I2C_EEread_byte(unsigned short adr)
{
    char ack, ret, tmp;

    while (IIC2.ICCR2.BIT.BBSY)
        // BBSY == 1の間
        ;

    // マスタ送信モードに設定
    IIC2.ICCR1.BIT.MST = 1; // マスタモード
    IIC2.ICCR1.BIT.TRS = 1; // 送信モード

    do
    {
        I2C_start();       // 開始条件を発行
        I2C_tx_byte(0xa0); // Control byteを送信
        while (!IIC2.ICSR.BIT.TEND)
            ;
        ack = IIC2.ICIER.BIT.ACKBR;
    } while (ack == 1);
    I2C_tx_byte((adr >> 8) & 0xff); // アドレス上位
    I2C_tx_byte(adr & 0xff);        // アドレス下位
    while (!IIC2.ICSR.BIT.TEND)
        ;

    I2C_start();
    I2C_tx_byte(0xa1); // Control byteを送信
    while (!IIC2.ICSR.BIT.TEND)
        ;
    IIC2.ICSR.BIT.TEND = 0;
    IIC2.ICCR1.BIT.TRS = 0; // 受信モード
    IIC2.ICSR.BIT.TDRE = 0;

    IIC2.ICIER.BIT.ACKBT = 1; // 返答ACK = 1 .. 終了を示す
    IIC2.ICCR1.BIT.RCVD = 1;  // 最終データ
    tmp = IIC2.ICDRR;         // ダミーリード

    while (!IIC2.ICSR.BIT.RDRF)
        // 受信完了を待つ
        ;

    I2C_stop();       // 停止条件を発行
    ret = IIC2.ICDRR; // 受信データ
    IIC2.ICCR1.BIT.RCVD = 0;

    // スレーブ受信モードに戻しておく
    IIC2.ICCR1.BIT.MST = 0; // スレーブモード
    IIC2.ICCR1.BIT.TRS = 0; // 受信モード

    return (ret);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
void main()
{
    int i, j, code;
    unsigned int adr;
    unsigned short tmp;

    PFC.PBCRL1.BIT.PB3MD = 4; // PB3をSDA端子に設定
    PFC.PBCRL1.BIT.PB2MD = 4; // PB2をSCL端子に設定

    STB.CR3.BIT._IIC2 = 0;  // スタンバイ解除
    IIC2.ICCR1.BIT.ICE = 1; // I2Cバス有効
    IIC2.ICCR1.BIT.CKS = 3; // 313kHz

    // EEPROMからfont_dataを読み出す
    adr = 0;
    for (i = 0; i < 95; i++)
    {
        for (j = 0; j < 32; j++)
        {
            tmp = I2C_EEread_byte(adr++);
            tmp <<= 8;
            tmp |= I2C_EEread_byte(adr++);
            font_data[i][j] = tmp;
        }
    }
    // 文字を簡易表示
    while (1)
    {
        printf("ASCII code = 0x");
        scanf("%x", &code);
        printf("%c\n", code);

        for (i = 0; i < 32; i++)
        {
            for (j = 0; j < 16; j++)
            {
                // -- 課題1 --
                if ((font_data[code][i] >> (15 - j)) % 2)
                    printf("*");
                else
                    printf(" ");
            }
            printf("\n");
        }
    }
}
