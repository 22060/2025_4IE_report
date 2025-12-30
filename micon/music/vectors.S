/*
 * SH7085 割り込みベクタテーブル
 */

.section .vect, "ax"

.global _vect0
_vect0:
    /* ベクタテーブル開始 */
    .long   _start          /* 0: リセット */
    .long   _null_int       /* 1: 予約 */
    .long   _null_int       /* 2: 予約 */
    .long   _null_int       /* 3: 予約 */
    .long   _null_int       /* 4: 非法命令 */
    .long   _null_int       /* 5: 予約 */
    .long   _null_int       /* 6: スロット非法命令 */
    .long   _null_int       /* 7: CPU命令非法命令 */
    .long   _null_int       /* 8: アドレスエラー(読出し) */
    .long   _null_int       /* 9: アドレスエラー(書込み) */
    .long   _null_int       /* 10: FPU例外 */
    .long   _null_int       /* 11: DMAC例外 */
    .long   _nmi_int        /* 12: NMI */
    .long   _null_int       /* 13: 予約 */
    .long   _null_int       /* 14: ユーザブレーク */
    .long   _null_int       /* 15: 予約 */

    /* 外部割り込み */
    .long   _null_int       /* 16: IRQ0 */
    .long   _null_int       /* 17: IRQ1 */
    .long   _null_int       /* 18: IRQ2 */
    .long   _null_int       /* 19: IRQ3 */
    .long   _null_int       /* 20: IRQ4 */
    .long   _null_int       /* 21: IRQ5 */
    .long   _null_int       /* 22: IRQ6 */
    .long   _null_int       /* 23: IRQ7 */

    /* 内蔵周辺割り込み開始 (24-) */
    .long   _null_int       /* 24 */
    .long   _null_int       /* 25 */
    .long   _null_int       /* 26 */
    .long   _null_int       /* 27 */
    .long   _null_int       /* 28 */
    .long   _null_int       /* 29 */
    .long   _null_int       /* 30 */
    .long   _null_int       /* 31 */

    /* MTU2タイマ割り込み (32-47) */
    .long   _mtu20_tgia     /* 32: MTU20 TGIA */
    .long   _null_int       /* 33: MTU20 TGIB */
    .long   _null_int       /* 34: MTU20 TGIC */
    .long   _null_int       /* 35: MTU20 TGID */
    .long   _null_int       /* 36: MTU20 TCIV */
    .long   _null_int       /* 37: MTU20 TGIE */
    .long   _null_int       /* 38: MTU20 TGIF */
    .long   _null_int       /* 39: 予約 */
    .long   _null_int       /* 40: MTU21 TGIA */
    .long   _null_int       /* 41: MTU21 TGIB */
    .long   _null_int       /* 42: MTU21 TCIV */
    .long   _null_int       /* 43: MTU21 TCIU */
    .long   _null_int       /* 44: MTU22 TGIA */
    .long   _null_int       /* 45: MTU22 TGIB */
    .long   _null_int       /* 46: MTU22 TCIV */
    .long   _null_int       /* 47: MTU22 TCIU */

    /* さらなる内蔵周辺割り込み (48-255は省略、全て_null_intに設定) */
    .rept 208
    .long   _null_int
    .endr

.text

/* スタートアップルーチン */
.global _start
_start:
    /* スタックポインタ初期化 */
    mov.l   stack_init, r15
    
    /* メイン関数呼び出し */
    mov.l   main_addr, r0
    jsr     @r0
    nop
    
    /* 無限ループ */
loop:
    bra     loop
    nop

.align 2
stack_init:
    .long   0x0C010000      /* スタック初期値 */
main_addr:
    .long   _main

/* MTU20 TGIA 割り込みハンドラ */
.global _mtu20_tgia
_mtu20_tgia:
    /* レジスタ退避 */
    sts.l   pr, @-r15
    stc.l   sr, @-r15
    mov.l   r0, @-r15
    mov.l   r1, @-r15
    mov.l   r2, @-r15
    mov.l   r3, @-r15
    mov.l   r4, @-r15
    mov.l   r5, @-r15
    mov.l   r6, @-r15
    mov.l   r7, @-r15
    
    /* 割り込み処理実行 */
    mov.l   audio_isr_addr, r0
    jsr     @r0
    nop
    
    /* レジスタ復帰 */
    mov.l   @r15+, r7
    mov.l   @r15+, r6
    mov.l   @r15+, r5
    mov.l   @r15+, r4
    mov.l   @r15+, r3
    mov.l   @r15+, r2
    mov.l   @r15+, r1
    mov.l   @r15+, r0
    ldc.l   @r15+, sr
    lds.l   @r15+, pr
    
    rte
    nop

.align 2
audio_isr_addr:
    .long   _audio_sample_isr

/* NMI割り込みハンドラ */
.global _nmi_int
_nmi_int:
    rte
    nop

/* デフォルト割り込みハンドラ */
.global _null_int
_null_int:
    rte
    nop
