/*
 * libmemes.h
 */

#ifndef _LIBMEMES_H
#define _LIBMEMES_H

#define	RAND_MAX	32767

#define	nop()	__asm__("nop")

void set_imask(int);
int get_imask(void);
void null_int(void);

int isprint(int);
int rand(void);
void srand(unsigned int);
int abs(int);

#endif
