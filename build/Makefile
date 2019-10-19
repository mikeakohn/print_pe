
#CC=gcc
CFLAGS=-Wall -O3
#CC=i386-mingw32msvc-gcc
#PATH=/opt/xmingw/bin:/usr/bin
VPATH=../src

OBJECTS= \
  cil.o \
  clsid.o \
  dos.o \
  file_header.o \
  fileio.o \
  optional_header.o \
  pe.o \
  pe_com.o \
  pe_debug.o \
  pe_exports.o \
  pe_imports.o \
  pe_vb.o \
  resource.o \
  section_header.o \
  table_cil_instr.o

default: $(OBJECTS)
	$(CC) -o ../print_pe ../src/print_pe.c \
	   $(OBJECTS) \
	   $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

