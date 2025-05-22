#pragma interrupt INT_CMT0_CMI0 // for CMT0
void INT_CMT0_CMI0()
{
    CMT0.CMCSR.BIT.CMF = 0; // 割り込み要求をクリア
    c2++;
    datas[0] = count % 10;
    datas[1] = (count / 10) % 10;
    datas[2] = (count / 100) % 10;
    disp_s_led(c2 % 3 + 1, datas[c2 % 3]);
}
#pragma interrupt INT_IRQ0 // for SW4
void INT_IRQ0()
{
    INTC.IRQSR.BIT.IRQ0F = 0; // 割り込み要求をクリア
    CMT.CMSTR.BIT.STR1 ^= 1;  // CMT1スタート
}
#pragma interrupt INT_IRQ1 // for SW5
void INT_IRQ1()
{
    INTC.IRQSR.BIT.IRQ1F = 0; // 割り込み要求をクリア

    if (INTC.IRQSR.BIT.IRQ0L == 1)
        // IRQ1端子 = 1 ... SW5 押されている
        LED5 ^= 1;
}
#pragma interrupt INT_CMT1_CMI1 // for CMT1
void INT_CMT1_CMI1()
{
    CMT1.CMCSR.BIT.CMF = 0; // 割り込み要求をクリア
    count++;
}