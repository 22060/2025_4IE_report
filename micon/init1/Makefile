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


# --------------------------------------------------------------------
TOPDIR	= /usr/local/sh-elf

INCLUDES = -I$(TOPDIR)/include
LIBDIR = -L$(TOPDIR)/sh-elf/lib
LIBDIR += -L$(TOPDIR)/lib/gcc/sh-elf/14.2.0
LIBDIR += -L.
ASFLAGS	=

CFLAGS = -nostdinc -nostdlib $(ASFLAGS) $(INCLUDES) -mrenesas -m2 -mb
LDFLAGS = -nostdlib --entry _vect0 $(LIBDIR)
LIBDEPS	= -lmemes -lgcc
OBJ_LDFLAGS = $(LDFLAGS) -T ram.map
OBJ_CFLAGS = $(CFLAGS) -O1

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
TGT := int1.mot
TGT_OBJ := $(TGT:.mot=.o)
TGT_SYM := $(TGT:.mot=.sym)

DEP_O = typedef.h libmemes.h
DEP_MYLIB_O = libmemes.h

STDOBJS = vectors.o startup.o
.PRECIOUS : vectors.o startup.o
MYLIB = libmemes.a

usage :
	@echo "make {int1 | clean}"

.PHONY : int1
int1 : int1.mot

%.mot : $(MYLIB) $(STDOBJS) %.o
	$(LD) --oformat srec $(OBJ_LDFLAGS) -o $@ $(STDOBJS) $(*F).o $(LIBDEPS)
	@$(LD) $(OBJ_LDFLAGS) -o debug.out $(STDOBJS) $(*F).o $(LIBDEPS)
	@$(NM) debug.out | grep -v \\.text | grep -v \\.data | grep -v \\.bss | grep -v compiled | sort > $(*F).sym
	@$(SIZE) debug.out

$(TGT_OBJ) : $(DEP_O)

$(MYLIB) : $(DEP_MYLIB_O)

.PHONY : clean
clean:
	rm -f $(TGT) $(TGT_OBJ) $(TGT_SYM) debug.out
