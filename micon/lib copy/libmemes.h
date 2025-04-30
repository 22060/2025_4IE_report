/*
 * libmemes.h
 */

#ifndef C13BADD9_2A5C_48C2_BDF1_ADB718698FB3
#define C13BADD9_2A5C_48C2_BDF1_ADB718698FB3

#ifndef _LIBMEMES_H
#define _LIBMEMES_H

#define nop() __asm__("nop")

int isprint(int);
void set_imask(int);
int get_imask(void);
void null_int(void);

#endif


#endif /* C13BADD9_2A5C_48C2_BDF1_ADB718698FB3 */
