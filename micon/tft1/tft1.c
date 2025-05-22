#include "7080S.H"

#define TFTDATA (*(volatile unsigned short *)0x08000000)
#define TFTCTRL (*(volatile unsigned short *)0x08000002)
#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

void TFT_clear(void)
{
    int i;
    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        TFTDATA = i % 320 <= 10 ? 0b0000000000011111 : 0x0000; // Clear screen with alternating colors
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

void main(void)
{
    init_CS2();
    TFT_On();
    TFT_clear();
    while (1)
        ;
}