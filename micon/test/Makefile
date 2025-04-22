#
#

CC	= sh-elf-gcc
AS	= sh-elf-as
LD	= sh-elf-ld
AR	= sh-elf-ar
NM	= sh-elf-nm
SIZE	= sh-elf-size
RANLIB	= sh-elf-ranlib
EXBIN	= exbin
BINEX	= binex
OUTPUTDIR = ./OUTPUT

# --------------------------------------------------------------------
TOPDIR	= /usr/local/sh-elf

INCLUDES = -I$(TOPDIR)/include
LIBDIR = -L$(TOPDIR)/sh-elf/lib
LIBDIR += -L$(TOPDIR)/lib/gcc/sh-elf/14.2.0
LIBDIR += -L../lib
ASFLAGS	=

CFLAGS = -nostdinc -nostdlib $(ASFLAGS) $(INCLUDES) -mrenesas -m2 -mb
LDFLAGS = -nostdlib --entry _vect0 $(LIBDIR)
LIBDEPS	= -lmemes -lgcc
OBJ_LDFLAGS = $(LDFLAGS) -T ../lib/ram.map
OBJ_CFLAGS = $(CFLAGS) -O0

# --------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .c .S .o .a

.c.o :
	$(CC) -c $(OBJ_CFLAGS) $<
.o.a :
	$(AR) rs $(*F).a $<
.c.s :
	$(CC) -S $(OBJ_CFLAGS) $<
.S.o :
	$(CC) -c $(OBJ_CFLAGS) $<

# --------------------------------------------------------------------
TGT = test

DEP_O = ../lib/typedef.h ../lib/libmemes.h
DEP_MYLIB_O = ../lib/libmemes.h

STDOBJS = ../lib/vectors.o ../lib/startup.o
MYLIB = ../lib/libmemes.a

OBJS = $(STDOBJS) $(TGT).o

$(TGT) : $(MYLIB) $(OBJS)
	@echo $(LD) --oformat srec $(OBJ_LDFLAGS) -o ./bin/$(TGT).mot $(OBJS) $(LIBDEPS)
	@$(LD) --oformat srec $(OBJ_LDFLAGS) -o ./bin/$(TGT).mot $(OBJS) $(LIBDEPS)
	@$(LD) $(OBJ_LDFLAGS) -o ./bin/debug.out $(OBJS) $(LIBDEPS)
	@$(NM) ./bin/debug.out | grep -v \\.text | grep -v \\.data | grep -v \\.bss | grep -v compiled | sort > ./bin/$(TGT).sym
	@$(SIZE) ./bin/debug.out
	@rm -f $(TGT).o


$(TGT).o : $(DEP_O)

$(MYLIB) : $(DEP_MYLIB_O)

clean:
	rm -f $(OBJS) $(TGT).sym $(TGT).mot debug.out test.o

