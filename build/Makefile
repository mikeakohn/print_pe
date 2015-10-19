
#CC=gcc
CFLAGS=-Wall -O3
#CC=i386-mingw32msvc-gcc
#PATH=/opt/xmingw/bin:/usr/bin
VPATH=../src

OBJECTS=cil.o fileio.o pe.o pe_debug.o res.o table_cil_instr.o

default: $(OBJECTS)
	$(CC) -o ../print_pe ../src/print_pe.c \
	   $(OBJECTS) \
	   $(CFLAGS)


%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

