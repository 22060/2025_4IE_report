#include "libmemes.h"
#include <7080S.H>
#define SW6 (PD.DR.BIT.B18)
#define SW5 (PD.DR.BIT.B17)
#define SW4 (PD.DR.BIT.B16)

#define LED6 (PE.DR.BIT.B11)
#define LED5 (PE.DR.BIT.B9)
#define LED_ON (0)
#define LED_OFF (1)
#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)
#define SPK (PE.DR.BIT.B0)

#define DIG1 (PE.DR.BIT.B3)
#define DIG2 (PE.DR.BIT.B2)
#define DIG3 (PE.DR.BIT.B1)

void disp_s_led(int num, int data);
int datas[3];
int count = 0;
int c2 = 0;

#pragma interrupt INT_CMT0_CMI0
void INT_CMT0_CMI0()
{
    CMT0.CMCSR.BIT.CMF = 0; // 割り込み要求をクリア
    c2++;
    datas[0] = count % 10;
    datas[1] = (count / 10) % 10;
    datas[2] = (count / 100) % 10;
    disp_s_led(c2 % 3 + 1, datas[c2 % 3]);
}
#pragma interrupt INT_IRQ0
void INT_IRQ0()
{
    INTC.IRQSR.BIT.IRQ0F = 0; // 割り込み要求をクリア
    CMT.CMSTR.BIT.STR1 ^= 1;  // CMT1スタート
}
#pragma interrupt INT_IRQ1
void INT_IRQ1()
{
    INTC.IRQSR.BIT.IRQ1F = 0; // 割り込み要求をクリア

    if (INTC.IRQSR.BIT.IRQ0L == 1)
        // IRQ1端子 = 1 ... SW5 押されている
        LED5 ^= 1;
}
#pragma interrupt INT_CMT1_CMI1
void INT_CMT1_CMI1()
{
    CMT1.CMCSR.BIT.CMF = 0; // 割り込み要求をクリア
    count++;
}
void main()
{
    PA.DR.BYTE.HL &= 0xF0;
    PA.DR.BYTE.HL |= 0x00;
    PFC.PDIORH.BIT.B16 = 0;
    PFC.PDIORH.BIT.B17 = 0;
    PFC.PDIORH.BIT.B18 = 0;

    PFC.PEIORL.BIT.B11 = 1; // PE11(LED6)端子を出力モードに設定
    // PFC.PEIORL.BIT.B9 = 1;     // PE9(LED5)端子を出力モードに設定
    PFC.PDCRH1.BIT.PD16MD = 2; // PD16(SW4)端子をIRQ0入力に設定
    // PFC.PDCRH1.BIT.PD17MD = 2; // PD17(SW5)端子をIRQ1入力に設定
    INTC.IRQCR.BIT.IRQ0S = 2; // 立ち上がり、立下りの両エッジ
    // INTC.IRQCR.BIT.IRQ1S = 2;  // 立ち上がり、立下りの両エッジ
    INTC.IPRA.BIT._IRQ0 = 8; // 割り込み優先レベル = 8
                             // INTC.IPRA.BIT._IRQ1 = 9;   // 割り込み優先レベル = 8

    PFC.PEIORL.BIT.B11 = 1; // PE11(LED6)端子を出力モードに設定

    STB.CR4.BIT._CMT = 0;    // CMTモジュールスタンバイの解除
    CMT0.CMCSR.BIT.CKS = 0;  // 1/512
    CMT0.CMCOR = 1250 - 1;   // 500ns
    CMT0.CMCSR.BIT.CMIE = 1; // コンペアマッチ割り込み許可
    INTC.IPRJ.BIT._CMT0 = 8; // 割り込み優先レベル = 8

    CMT1.CMCSR.BIT.CKS = 3;  // 1/512
    CMT1.CMCOR = 3906 - 1;   // 500ms
    CMT1.CMCSR.BIT.CMIE = 1; // コンペアマッチ割り込み許可
    INTC.IPRJ.BIT._CMT1 = 8; // 割り込み優先レベル = 8

    set_imask(7); // マスクビット = 7

    LED6 = LED_OFF;         // LED6初期状態
    PFC.PEIORL.BIT.B0 = 1;  // PE0(SPK) .. 出力
    CMT.CMSTR.BIT.STR0 = 1; // CMT0スタート
    while (1)
        ;
}
void disp_s_led(int num, int data)
{
    PA.DR.BYTE.HL &= 0xF0;
    PA.DR.BYTE.HL |= data;
    switch (num)
    {
    case 1:
        DIG1 = 1;
        DIG2 = 0;
        DIG3 = 0;
        break;
    case 2:
        DIG1 = 0;
        DIG2 = 1;
        DIG3 = 0;
        break;
    case 3:
        DIG1 = 0;
        DIG2 = 0;
        DIG3 = 1;
        break;
    }
}