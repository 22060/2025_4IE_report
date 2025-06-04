CC	= sh-elf-gcc
AS	= sh-elf-as
LD	= sh-elf-ld
AR	= sh-elf-ar
NM	= sh-elf-nm
SIZE	= sh-elf-size

TOPDIR	= /usr/local/sh-elf
INCLUDES = -I$(TOPDIR)/include
LIBDIR = -L$(TOPDIR)/sh-elf/lib
LIBDIR += -L$(TOPDIR)/lib/gcc/sh-elf/14.2.0
LIBDIR += -L../lib

ASFLAGS	=
CFLAGS = -nostdinc -nostdlib $(ASFLAGS) $(INCLUDES) -mrenesas -m2 -mb
LDFLAGS = -nostdlib --entry _vect0 $(LIBDIR)
LIBDEPS	= ../lib/libmemes.a -lgcc
OBJ_LDFLAGS = $(LDFLAGS) -T ../lib/ram.map
OBJ_CFLAGS = $(CFLAGS) -O0

.SUFFIXES:
.SUFFIXES: .c .S .o .a

.c.o :
	$(CC) -c $(OBJ_CFLAGS) $<

.o.a :
	$(AR) rs $(*F).a $<

.S.o :
	$(CC) -c $(OBJ_CFLAGS) $<

TGT = i2c3

STDOBJS = ../lib/vectors.o ../lib/startup.o
OBJS = $(STDOBJS) $(TGT).o

$(TGT) : $(OBJS)
	@mkdir -p ./bin
	@$(LD) --oformat srec $(OBJ_LDFLAGS) -o ./bin/$(TGT).mot $(OBJS) $(LIBDEPS)
	@$(LD) $(OBJ_LDFLAGS) -o ./bin/debug.out $(OBJS) $(LIBDEPS)
	@$(NM) ./bin/debug.out | grep -v \\.text | grep -v \\.data | grep -v \\.bss | grep -v compiled | sort > ./bin/$(TGT).sym
	@$(SIZE) ./bin/debug.out
	@rm -f $(TGT).o

clean:
	rm -f $(TGT).o ./bin/$(TGT).mot ./bin/$(TGT).sym ./bin/debug.out
