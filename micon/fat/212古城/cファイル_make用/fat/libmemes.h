/*
 * libmemes.h
 */

#ifndef F6E8F08B_2E93_4061_B7EC_918EBEE4EAC0
#define F6E8F08B_2E93_4061_B7EC_918EBEE4EAC0

#ifndef _LIBMEMES_H
#define _LIBMEMES_H

#define nop() __asm__("nop")

int isprint(int);
void set_imask(int);
int get_imask(void);
void null_int(void);
void INT_CMT1_CMI0(void);

#endif


#endif /* F6E8F08B_2E93_4061_B7EC_918EBEE4EAC0 */
