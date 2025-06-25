#include "libmemes.h"
#include <7080S.H>

#define LCD_RS (PA.DR.BIT.B22)
#define LCD_E (PA.DR.BIT.B23)
#define LCD_RW (PD.DR.BIT.B23)
#define LCD_DATA (PD.DR.BYTE.HH)

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
void LCD_inst(char inst)
{
    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 1;
    LCD_DATA = inst;
    wait_us(1);
    LCD_E = 0;
    wait_us(40);
}

void LCD_data(char data)
{
    LCD_E = 0;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E = 1;
    LCD_DATA = data;
    wait_us(1);
    LCD_E = 0;
    wait_us(40);
}

void init_LCD()
{
    PFC.PAIORH.BIT.B22 = 1;
    PFC.PAIORH.BIT.B23 = 1;
    PFC.PDIORH.BIT.B23 = 1;
    PFC.PDIORH.BYTE.H = 0xff;

    wait_us(45000);
    LCD_inst(0x30);
    wait_us(4100);
    LCD_inst(0x30);
    wait_us(100);
    LCD_inst(0x30);

    LCD_inst(0x38);
    LCD_inst(0x08);
    LCD_inst(0x01);
    wait_us(1640);
    LCD_inst(0x06);
    LCD_inst(0x0c);
}

void LCD_cursor(unsigned int x, unsigned int y)
{
    if (x > 15)
        x = 15;

    if (y > 1)
        y = 1;

    LCD_inst(0x80 | x | y << 6);
}

void LCD_cls()
{
    LCD_inst(0x01);
    wait_us(1640);
}

void LCD_putch(char ch)
{
    LCD_data(ch);
}

void LCD_putstr(char *str)
{
    char ch;

    while (ch = *str++)
        LCD_putch(ch);
}

// ------------------------------------------------------------
void sci1_putch(char ch)
{
    // -- 演習１ --
    while (SCI1.SCSSR.BIT.TDRE == 0)
        ;                    // 送信可能になるまで待つ
    SCI1.SCTDR = ch;         // 送信データレジスタにデータをセット
    SCI1.SCSSR.BIT.TDRE = 0; // 送信完了フラグをクリア
    while (SCI1.SCSSR.BIT.TEND == 0)
        ; // 送信完了になるまで待つ
}

void sci1_putstr(char *str)
{
    // -- 演習２ --
    while (*str)
    {
        sci1_putch(*str++); // 文字列の各文字を送信
    }
    sci1_putch('\r'); // 改行コードを送信
    sci1_putch('\n'); // 改行コードを送信
}

char sci1_getch()
{
    char buf;
    // -- 演習３ --
    while (SCI1.SCSSR.BIT.RDRF == 0)
        ;
    buf = SCI1.SCRDR;
    SCI1.SCSSR.BIT.RDRF = 0;
    return buf; // 受信データを返す
}

// ------------------------------------------------------------
void main(void)
{
    init_CMT0();
    init_LCD();

    // SCI1 初期化
    STB.CR3.BIT._SCI1 = 0;
    SCI1.SCSCR.BYTE &= 0x03;
    SCI1.SCSCR.BIT.CKE = 0;
    SCI1.SCSMR.BYTE = 0;
    SCI1.SCBRR = 15;          // 38400bps
    wait_us(104);             // 104us待つ
    PFC.PACRL1.BIT.PA3MD = 1; // RxD1端子に設定
    PFC.PACRL2.BIT.PA4MD = 1; // TxD1端子に設定
    SCI1.SCSCR.BYTE |= 0x30;  // TE, RE = 1

    sci1_putch('A');
    sci1_putstr("\r\nBCDEFG");

    while (1)
        LCD_putch(sci1_getch());
}
