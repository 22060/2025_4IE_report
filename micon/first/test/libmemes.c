/*
 * MEMEs 用標準ライブラリ
 *
 */

#include "libmemes.h"

// ------------------------------------------------------------
void zero_bss(void *start, void *end);
void main(void);

// ------------------------------------------------------------
/* main() を実行する前の処理 */
extern char sbss, ebss;
void before_main(void)
{
  zero_bss(&sbss, &ebss);

  main();

  while (1)
	;
}


// ------------------------------------------------------------
int isprint(int c)
{
  if ((c >= 0x20) && (c < 0x7f))
	return (1);
  else
	return (0);
}

void zero_bss(void *start, void *end)
{
  while (start < end)
	*(char*)start++ = 0;
}

void set_imask(int level)
{
  __asm__ volatile("\n"
	"	stc	sr, r0		\n"
	"	mov.l	L_MASK, r1	\n"
	"	and	r1, r0		\n"
	"	shll	%0		\n"
	"	shll	%0		\n"
	"	shll	%0		\n"
	"	shll	%0		\n"
	"	or	%0, r0		\n"
	"	ldc	r0, sr		\n"
	:
	: "r"(level)
	: "r0", "r1");
}

int get_imask()
{
  int ret;

  __asm__ volatile("\n"
	"	stc	sr, %0		\n"
	"	mov.l	L_MASK, r0	\n"
	"	not	r0, r0		\n"
	"	and	r0, %0		\n"
	"	shlr	%0		\n"
	"	shlr	%0		\n"
	"	shlr	%0		\n"
	"	shlr	%0		\n"
	"	bra	L_FIN		\n"
	"	nop			\n"
	".align	2			\n"
	"L_MASK:.long	0xffffff0f	\n"
	"L_FIN:				\n"
	: "=r"(ret)
	:
	: "r0");

  return (ret);
}


#pragma interrupt
void null_int()
{
  while(1);
}
