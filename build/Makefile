
#CC=gcc
CFLAGS=-Wall -O3
#CC=i386-mingw32msvc-gcc
#PATH=/opt/xmingw/bin:/usr/bin
VPATH=../src

OBJECTS=cil.o dos.o fileio.o pe.o pe_clsid.o pe_com.o pe_debug.o pe_image_headers.o pe_section_headers.o pe_resource.o pe_vb.o table_cil_instr.o

default: $(OBJECTS)
	$(CC) -o ../print_pe ../src/print_pe.c \
	   $(OBJECTS) \
	   $(CFLAGS)


%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

