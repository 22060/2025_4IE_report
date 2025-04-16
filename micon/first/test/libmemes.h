/*
 * libmemes.h
 */

#ifndef _LIBMEMES_H
#define _LIBMEMES_H

#define	nop()	__asm__("nop")

int isprint(int);
void set_imask(int);
int get_imask(void);
void null_int(void);

#endif
