
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

OBJECTS_MINIDUMP= \
  fileio.o \
  minidump.o \
  minidump_stream_type.o

OBJECTS_PDB= \
  fileio.o \
  pdb.o

default: $(OBJECTS) $(OBJECTS_MINIDUMP) $(OBJECTS_PDB)
	$(CC) -o ../print_pe ../src/print_pe.c \
	   $(OBJECTS) \
	   $(CFLAGS)
	$(CC) -o ../print_minidump ../src/print_minidump.c \
	   $(OBJECTS_MINIDUMP) \
	   $(CFLAGS)
	$(CC) -o ../print_pdb ../src/print_pdb.c \
	   $(OBJECTS_PDB) \
	   $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

