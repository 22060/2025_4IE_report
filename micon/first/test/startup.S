/*
 * startup.S
 */

/* リセット解除後の処理 */
	.global	_vect0
_vect0:
	mov.l	L_SR, r0
	ldc	r0, sr		// 割り込み禁止

	mov.l	L_VBR, r0
	ldc	r0, vbr		// VBR を RAM 先頭に

	mov.l	L_MAIN, r0
	mov.l	L_STACK, r15	// スタックポインタを内蔵 RAM に
	jmp	@r0		// before_main() へジャンプ
	nop

	.align	2
L_MAIN:		.long	_before_main
L_STACK:	.long	0xffffc000
L_VBR:		.long	0x04000000
L_SR:		.long	0x000000f0
