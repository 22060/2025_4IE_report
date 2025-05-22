#include "libmemes.h"
#include <7080S.H>
#include "typedef.h"
#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)

#define TFTDATA (*(volatile unsigned short *)0x08000000)
#define TFTCTRL (*(volatile unsigned short *)0x08000002)
#define _COL_WHITE (0xFFFF)
#define _COL_RED (0xF800)
volatile _UWORD FrameBuf[320 * 240];
void TFT_draw_point(_UWORD x_pix, _UWORD y_pix, _UWORD p_color)
{
    _SINT i;
    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        if (i == y_pix * 320 + x_pix)
        {
            FrameBuf[i] = p_color;
        }
        else
        {
            FrameBuf[i] = _COL_WHITE;
        }
    }
}

void TFT_clear(void)
{
    _SINT i;
    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        FrameBuf[i] = _COL_WHITE;
        TFTDATA = _COL_WHITE;
    }
}

void TFT_dot(void)
{
    int x, y;
    _SINT i;

    x = 160;
    y = 120;

    FrameBuf[y * 320 + x - 1] = _COL_RED;
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
void TFT_draw_screen()
{
    _SINT i;

    TFTCTRL = 0x4001;
    for (i = 0; i < (320 * 240); i++)
    {
        TFTDATA = FrameBuf[i];
    }
}
void TFT_draw_10pix_square(void)
{
    _SINT x_pix, y_pix, i;
    _UWORD x, y, w;

    x = 160;
    y = 220;
    w = 10;

    for (x_pix = x; x_pix < (x + w); x_pix++)
    {
        for (y_pix = y; y_pix < (y + w); y_pix++)
        {
            FrameBuf[y_pix * 320 + x_pix] = _COL_RED;
        }
    }
}
void main(void)
{
    init_CS2();
    TFT_On();
    TFT_clear();
    TFT_draw_point(160, 120, _COL_RED);
    TFT_draw_10pix_square();
    TFT_draw_screen();
    while (1)
        ;
}
