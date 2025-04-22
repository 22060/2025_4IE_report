/*
 * int1.c
 */

#include <7080S.H>
#include "libmemes.h"

#define USE_INT

#define printf ((int (*)(const char *, ...))0x00007c7c)

#define SW6 (PD.DR.BIT.B18)
#define SW5 (PD.DR.BIT.B17)
#define SW4 (PD.DR.BIT.B16)

#define LED6 (PE.DR.BIT.B11)
#define LED5 (PE.DR.BIT.B9)
#define LED_ON (0)
#define LED_OFF (1)

// ------------------------------------------------------------
// IRQ0(SW4)の割り込み処理関数
#pragma interrupt INT_IRQ0
void INT_IRQ0()
{
    INTC.IRQSR.BIT.IRQ0F = 0; // 割り込み要求をクリア

    if (INTC.IRQSR.BIT.IRQ0L == 1)
        // IRQ0端子 = 1 ... SW4 押されている
        LED6 = LED_ON;
    else
        LED6 = LED_OFF;
}

// ------------------------------------------------------------
#ifdef USE_INT
// 割り込みを使用するときのメイン関数
void main()
{
    PFC.PEIORL.BIT.B11 = 1;    // PE11(LED6)端子を出力モードに設定
    PFC.PDCRH1.BIT.PD16MD = 2; // PD16(SW4)端子をIRQ0入力に設定
    INTC.IRQCR.BIT.IRQ0S = 3;  // 立ち上がり、立下りの両エッジ
    INTC.IPRA.BIT._IRQ0 = 15;  // 割り込み優先レベル = 8

    set_imask(7); // マスクビット = 7

    LED6 = LED_OFF; // LED6初期状態

    printf("-- USE INT --\n");
    while (1)
    {
        printf("SW4 = %d\n", SW4);
    }
}

// ------------------------------------------------------------
#else
// 割り込みを使用しないときのメイン関数
void main()
{
    PFC.PEIORL.BIT.B11 = 1; // PE11(LED6)端子を出力モードに設定

    printf("-- not USE INT --\n");
    while (1)
    {
        if (SW4 == 1)
            LED6 = LED_ON;
        else
            LED6 = LED_OFF;
    }
}
#endif
