/*
 * i2c23.c
 */

#include <7080S.H>
#include "libmemes.h"

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

// ------------------------------------------------------------
#define ACK 1
#define NAK -1

// ------------------------------------------------------------
// -- TFT 液晶関係 --
//
#define TFTDATA (*(volatile unsigned short *)0x08000000)
#define TFTCTRL (*(volatile unsigned short *)0x08000002)
#define _COL_WHITE (0xFFFF)
#define _COL_RED (0xF800)
#define _COL_GREEN (0x07E0)
#define _COL_BLUE (0x001F)
#define _COL_BLACK (0x0000)

volatile unsigned short FrameBuf[320 * 240];
volatile unsigned short board[8 * 8];
unsigned short cursor[2] = {0, 0};

int koma[32 * 2] = {
    
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0003, 0xC000,
        0x000F, 0xF000,
        0x003F, 0xFC00,
        0x00FF, 0xFF00,
        0x01FF, 0xFF80,
        0x01FF, 0xFF80,
        0x03FF, 0xFFC0,
        0x03FF, 0xFFC0,
        0x07FF, 0xFFE0,
        0x07FF, 0xFFE0,
        0x0FFF, 0xFFF0,
        0x0FFF, 0xFFF0,
        0x0FFF, 0xFFF0,
        0x0FFF, 0xFFF0,
        0x07FF, 0xFFE0,
        0x07FF, 0xFFE0,
        0x03FF, 0xFFC0,
        0x03FF, 0xFFC0,
        0x01FF, 0xFF80,
        0x01FF, 0xFF80,
        0x00FF, 0xFF00,
        0x003F, 0xFC00,
        0x000F, 0xF000,
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0000, 0x0000,
    }

void
TFT_draw_point(unsigned short x_pix, unsigned short y_pix, unsigned short p_color)
{
    FrameBuf[y_pix * 320 + x_pix] = p_color;
}

// FrameBuf[]のデータをTFTへ転送する
void TFT_draw_screen(void)
{
    int i;

    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        TFTDATA = FrameBuf[i];
    }
}

void TFT_clear(void)
{
    int i;

    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        TFTDATA = _COL_WHITE;
        FrameBuf[i] = _COL_WHITE;
    }
}

void TFT_On(void)
{
    TFTCTRL = 0x4000;
}

void init_CS2(void)
{
    BSC.CS2BCR.LONG = 0x12490400;
    BSC.CS2WCR = 0x000302C0;
    PFC.PACRL4.BIT.PA15MD = 1;
    PFC.PACRL2.BIT.PA6MD = 2;
}

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

    //	IIC2.ICSR.BIT.TEND = 0;		////////////
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
void TFT_putch(unsigned short x, unsigned short y, char ch, unsigned short fg_color, unsigned short bg_color)
{
    int i, j;

    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 16; j++)
        {
            if (font_data[ch - 32][i] & (1 << (15 - j)))
            {
                // fg_colorを設定
                TFT_draw_point(x + j, y + i, fg_color);
            }
            else
            {
                // bg_colorを設定
                TFT_draw_point(x + j, y + i, bg_color);
            }
        }
    }
}

void TFT_putstr(unsigned short x, unsigned short y, char *str, unsigned short fg_color, unsigned short bg_color)
{
    char ch;
    int i, j, k;
    k = 0; // 文字の位置をずらす

    while (ch = *str++)
    {
        // printf("ch = %c\n", ch);
        // -- 課題３ --
        TFT_putch(x + k * 16, y, ch, fg_color, bg_color);
        k++; // 文字の位置をずらす
    }
}
void TFT_draw_line(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short p_color)
{
    int i;
    int j;
    TFTCTRL = 0x4001;
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            if (x + i < 320 && y + j < 240)
            {
                FrameBuf[(y + j) * 320 + (x + i)] = p_color;
            }
        }
    }
}
void GAME_draw_board(void)
{
    int i, j;
    unsigned short x, y;

    x = 40;
    y = 0;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            switch (board[i + j * 8])
            {
            case 0:
                // 何もしない
                break;
            case 1:
                for (i = 0; i < 30; i++)
                {
                    for (j = 0; j < 30; j++)
                    {
                        if (board[i + j * 8] & (1 << (15 - j)))
                        {
                            // fg_colorを設定
                            TFT_draw_point(x + j, y + i, fg_color);
                        }
                        else
                        {
                            // bg_colorを設定
                            TFT_draw_point(x + j, y + i, bg_color);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}
// ------------------------------------------------------------
// ------------------------------------------------------------
void main()
{
    int i, j, code, k;
    unsigned int adr;
    unsigned short tmp;
    k = 0;

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

    init_CS2();
    TFT_On();
    TFT_clear();

    TFT_draw_line(40, 0, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 30, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 60, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 90, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 120, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 150, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 180, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 210, 240, 1, _COL_BLACK);
    TFT_draw_line(40, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(70, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(100, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(130, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(160, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(190, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(220, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(250, 0, 1, 240, _COL_BLACK);
    TFT_draw_line(280, 0, 1, 240, _COL_BLACK);

    TFT_draw_screen();

    while (1)
    {
    };
}
