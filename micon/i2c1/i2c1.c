/*
 * i2c11.c
 */

#include <7080S.H>
#include "libmemes.h"

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

// ------------------------------------------------------------
#define ACK 1
#define NAK -1

// ------------------------------------------------------------
// 開始条件(start condition)を発行する
void I2C_start(void)
{
    // この関数を作成
    IIC2.ICCR2.BYTE = (IIC2.ICCR2.BYTE & 0xbf) | 0x80;
    while (IIC2.ICSR.BIT.TDRE == 0)
        ; // TDREが1になるまで待つ
}

// 停止条件(stop condition)を発行する
void I2C_stop(void)
{
    // この関数を作成
    IIC2.ICSR.BIT.STOP = 0;  // 停止条件をクリア
    IIC2.ICCR2.BYTE &= 0x3f; // 停止条件を発行
    while (IIC2.ICSR.BIT.STOP == 1)
        ; // 停止条件がクリアされるまで待つ
}

int I2C_tx_byte(char data)
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

    while (!IIC2.ICSR.BIT.RDRF) // 受信完了を待つ
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
    unsigned int adr, data, i;
    char tmp;

    PFC.PBCRL1.BIT.PB3MD = 4; // PB3をSDA端子に設定
    PFC.PBCRL1.BIT.PB2MD = 4; // PB2をSCL端子に設定

    STB.CR3.BIT._IIC2 = 0;  // スタンバイ解除
    IIC2.ICCR1.BIT.ICE = 1; // I2Cバス有効
    IIC2.ICCR1.BIT.CKS = 3; // 313kHz

    printf("write = 0xa5\n");
    I2C_EEwrite_byte(0x00, 0xa5);
    printf("readback = 0x%02x", I2C_EEread_byte(0x00));

    while (1)
    {
        printf("\ncommand > ");
        scanf("%s", &tmp);
        scanf("%x", &adr);
        switch (tmp)
        {
        case 'W': // write
            scanf("%x", &data);
            printf("command = %c, adr = %04x\n", tmp, adr);
            I2C_EEwrite_byte(adr, data);
            printf("write %02x to %04x\n", data, adr);
            break;
        case 'R': // read
            tmp = I2C_EEread_byte(adr);
            printf("read %02x from %04x\n", tmp, adr);
            break;
        case 'D': // dump
            for (i = 0; i < 0x40; i++)
            {
                if (!(i % 16))
                {
                    printf("\n");
                    printf("%04x : ", adr + i);
                }
                printf("%02x ", I2C_EEread_byte(adr + i));
            }
            break;
        default:
            printf("unknown command\n");
        }
    }
}
