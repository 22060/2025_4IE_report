/*
 * libmemes.h
 */

#ifndef CC60E742_4145_4AD3_98BA_F137A0E4FCEC
#define CC60E742_4145_4AD3_98BA_F137A0E4FCEC

#ifndef _LIBMEMES_H
#define _LIBMEMES_H

#define nop() __asm__("nop")

int isprint(int);
void set_imask(int);
int get_imask(void);
void null_int(void);

#endif


#endif /* CC60E742_4145_4AD3_98BA_F137A0E4FCEC */
