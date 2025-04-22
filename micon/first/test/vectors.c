/*
 * vectors.c
 */

#include "libmemes.h"

void vect0(void);

struct vtable_t
{
  void (*func_cold)();
  int *stack_cold;
  void (*func_warm)();
  int *stack_warm;
  void (*(func[256 - 4]))();
} v_table[] __attribute__((section(".vector"))) = {

    &vect0, (void *)0xffff4000, /*	RESET */
    &vect0, (void *)0xffff4000, /*	Warm RESET */
    null_int,                   /*	不当命令 */
    null_int,                   /* 5	システム予約 */
    null_int,                   /*	スロット不当命令 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	CPUアドレスエラー */
    null_int,                   /* 10	DMACアドレスエラー */
    null_int,                   /*	NMI */
    null_int,                   /*	ユーザブレーク */
    null_int,                   /*	H-UDI */
    null_int,                   /*	システム予約 */
    null_int,                   /* 15	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /* 20	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /* 25	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /* 30	システム予約 */
    null_int,                   /*	システム予約 */
    null_int,                   /*	TRAP32 */
    null_int,                   /*	TRAP33 */
    null_int,                   /*	TRAP34 */
    null_int,                   /* 35	TRAP35 */
    null_int,                   /*	TRAP36 */
    null_int,                   /*	TRAP37 */
    null_int,                   /*	TRAP38 */
    null_int,                   /*	TRAP39 */
    null_int,                   /* 40	TRAP40 */
    null_int,                   /*	TRAP41 */
    null_int,                   /*	TRAP42 */
    null_int,                   /*	TRAP43 */
    null_int,                   /*	TRAP44 */
    null_int,                   /* 45	TRAP45 */
    null_int,                   /*	TRAP46 */
    null_int,                   /*	TRAP47 */
    null_int,                   /*	TRAP48 */
    null_int,                   /*	TRAP49 */
    null_int,                   /* 50	TRAP50 */
    null_int,                   /*	TRAP51 */
    null_int,                   /*	TRAP52 */
    null_int,                   /*	TRAP53 */
    null_int,                   /*	TRAP54 */
    null_int,                   /* 55	TRAP55 */
    null_int,                   /*	TRAP56 */
    null_int,                   /*	TRAP57 */
    null_int,                   /*	TRAP58 */
    null_int,                   /*	TRAP59 */
    null_int,                   /* 60	TRAP60 */
    null_int,                   /*	TRAP61 */
    null_int,                   /*	TRAP62 */
    null_int,                   /*	TRAP63 */
    null_int,                   /*	IRQ0 */
    null_int,                   /* 65	IRQ1 */
    null_int,                   /*	IRQ2 */
    null_int,                   /*	IRQ3 */
    null_int,                   /*	IRQ4 */
    null_int,                   /*	IRQ5 */
    null_int,                   /* 70	IRQ6 */
    null_int,                   /*	IRQ7 */
    null_int,                   /*	DMAC0 DEI0 */
    null_int,
    null_int,
    null_int, /* 75 */
    null_int, /*	DMAC1 EDI1 */
    null_int,
    null_int,
    null_int,
    null_int, /* 80	DMAC2 DEI2 */
    null_int,
    null_int,
    null_int,
    null_int, /*	DMAC3 DEI3 */
    null_int, /* 85 */
    null_int,
    null_int,
    null_int, /*	MTU2_0 TGIA0 */
    null_int, /*	MTU2_0 TGIB0 */
    null_int, /* 90	MTU2_0 TGIC0 */
    null_int, /*	MTU2_0 TGID0 */
    null_int, /*	MTU2_0 TCIV0 */
    null_int, /*	MTU2_0 TGIE0 */
    null_int, /*	MTU2_0 TGIF0 */
    null_int, /* 95 */
    null_int, /*	MTU2_1 TGIA1 */
    null_int, /*	MTU2_1 TGIB1 */
    null_int,
    null_int,
    null_int, /* 100	MTU2_1 TCIV1 */
    null_int, /*	MTU2_1 TCIU1 */
    null_int,
    null_int,
    null_int, /*	MTU2_2 TGIA2 */
    null_int, /* 105	MTU2_2 RGIB2 */
    null_int,
    null_int,
    null_int, /*	MTU2_2 TCIV2 */
    null_int, /*	MTU2_2 TCIU2 */
    null_int, /* 110 */
    null_int,
    null_int, /*	MTU2_3 TGIA3 */
    null_int, /*	MTU2_3 TGIB3 */
    null_int, /*	MTU2_3 TGIC3 */
    null_int, /* 115	MTU2_3 TGID3 */
    null_int, /*	MTU2_3 TCIV3 */
    null_int,
    null_int,
    null_int,
    null_int, /* 120	MTU2_4 TGIA4 */
    null_int, /*	MTU2_4 TGIB4 */
    null_int, /*	MTU2_4 TGIC4 */
    null_int, /*	MTU2_4 TGID4 */
    null_int, /*	MTU2_4 TCIV4 */
    null_int, /* 125 */
    null_int,
    null_int,
    null_int, /*	MTU2_5 TGIU5 */
    null_int, /*	MTU2_5 TGIV5 */
    null_int, /* 130	MTU2_5 TGIW5 */
    null_int,
    null_int, /*	POE OEI1 */
    null_int, /*	POE OEI3 */
    null_int,
    null_int, /* 135 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 140 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 145 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 150 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 155 */
    null_int, /*	IIC2 IINAKI */
    null_int,
    null_int,
    null_int,
    null_int, /* 160	MTU2S_3 TGIA3S */
    null_int, /*	MTU2S_3 TGIB3S */
    null_int, /*	MTU2S_3 TGIC3S */
    null_int, /*	MTU2S_3 TGID3S */
    null_int, /*	MTU2S_3 TCIV3S */
    null_int, /* 165 */
    null_int,
    null_int,
    null_int, /*	MTU2S_4 TGIA4S */
    null_int, /*	MTU2S_4 TGIB4S */
    null_int, /* 170	MTU2S_4 TGIC4S */
    null_int, /*	MTU2S_4 TGID4S */
    null_int, /*	MTU2S_4 TCIV4S */
    null_int,
    null_int,
    null_int, /* 175 */
    null_int, /*	MTU2S_5 TGIU5S */
    null_int, /*	MTU2S_5 TGIV5S */
    null_int, /*	MTU2S_5 TGIW5S */
    null_int,
    null_int, /* 180	POE OEI2 */
    null_int,
    null_int,
    null_int,
    null_int, /*	CMT0 CMI0 */
    null_int, /* 185 */
    null_int,
    null_int,
    null_int, /*	CMT1 CMI1 */
    null_int,
    null_int, /* 190 */
    null_int,
    null_int, /*	BSC CMI */
    null_int,
    null_int,
    null_int, /* 195 */
    null_int, /*	WDT ITI */
    null_int,
    null_int,
    null_int,
    null_int, /* 200	AD0 ADI0 */
    null_int, /*	AD1 ADI1 */
    null_int,
    null_int,
    null_int, /*	AD2 ADI2 */
    null_int, /* 205 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 210 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 215 */
    null_int, /*	SCI0 ERI0 */
    null_int, /*	SCI0 RXI0 */
    null_int, /*	SCI0 TXI0 */
    null_int, /*	SCI0 TEI0 */
    null_int, /* 220	SCI1 ERI1 */
    null_int, /*	SCI1 RXI1 */
    null_int, /*	SCI1 TXI1 */
    null_int, /*	SCI1 TEI1 */
    null_int, /*	SCI2 ERI2 */
    null_int, /* 225	SCI2 RXI2 */
    null_int, /*	SCI2 TXI2 */
    null_int, /*	SCI2 TEI2 */
    null_int, /*	SCIF ERIF */
    null_int, /*	SCIF RXIF */
    null_int, /* 230	SCIF BRIF */
    null_int, /*	SCIF TXIF */
    null_int, /*	SSU SSERI */
    null_int, /*	SSU SSRXI */
    null_int, /*	SSU SSTXI */
    null_int, /* 235 */
    null_int, /*	IIC2 IITEI */
    null_int, /*	IIC2 IISTPI */
    null_int, /*	IIC2 IITXI */
    null_int, /*	IIC2 IIRXI */
    null_int, /* 240 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 245 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 250 */
    null_int,
    null_int,
    null_int,
    null_int,
    null_int, /* 255 */
};
