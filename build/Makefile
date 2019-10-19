
#CC=gcc
CFLAGS=-Wall -O3
#CC=i386-mingw32msvc-gcc
#PATH=/opt/xmingw/bin:/usr/bin
VPATH=../src

OBJECTS= \
  cil.o \
  clsid.o \
  com.o \
  debug_directory.o \
  debug_section.o \
  dos.o \
  exports.o \
  file_header.o \
  fileio.o \
  imports.o \
  optional_header.o \
  pe.o \
  resource.o \
  section_header.o \
  table_cil_instr.o \
  vb.o

default: $(OBJECTS)
	$(CC) -o ../print_pe ../src/print_pe.c \
	   $(OBJECTS) \
	   $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

