/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "file_header.h"

int file_header_read(struct file_header_t *file_header, FILE *in)
{
  file_header->Machine = read_uint16(in);
  file_header->NumberOfSections = read_uint16(in);
  file_header->TimeDateStamp = read_uint32(in);
  file_header->PointerToSymbolTable = read_uint32(in);
  file_header->NumberOfSymbols = read_uint32(in);
  file_header->SizeOfOptionalHeader = read_uint16(in);
  file_header->Characteristics = read_uint16(in);

  return 0;
}

int file_header_print(struct file_header_t *file_header)
{
  printf("---------------------------------------------\n");
  printf("Image File Header\n");
  printf("---------------------------------------------\n");

  const char *machine;

  switch(file_header->Machine)
  {
    case 0x14c: machine = "i386"; break;
    case 0x14d: machine = "i486"; break;
    case 0x14e: machine = "i586"; break;
    case 0x160: machine = "MIPS R3000 Big Endian"; break;
    case 0x162: machine = "MIPS R3000 Little Endian"; break;
    case 0x166: machine = "MIPS R4000 Little Endian"; break;
    case 0x168: machine = "MIPS R10000 Little Endian"; break;
    case 0x169: machine = "MIPS Little Endian WCE v2"; break;
    case 0x184: machine = "Alpha"; break;
    case 0x1a2: machine = "Hitachi SH3"; break;
    case 0x1a3: machine = "Hitachi SH3 DSP"; break;
    case 0x1a8: machine = "Hitachi SH5"; break;
    case 0x1c0: machine = "ARM Little Endian"; break;
    case 0x1c2: machine = "Thumb"; break;
    case 0x1c4: machine = "ARM Thumb-2 Little Endian"; break;
    case 0x1d3: machine = "Matsushita AM33"; break;
    case 0x1f0: machine = "PPC Little Endian"; break;
    case 0x1f1: machine = "PPC w/ FPU"; break;
    case 0x266: machine = "MIPS16"; break;
    case 0x366: machine = "MIPS w/ FPU"; break;
    case 0x466: machine = "MIPS16 w/ FPU"; break;
    case 0xebc: machine = "EFI Byte Code"; break;
    case 0x5032: machine = "RISC-V 32 bit"; break;
    case 0x5064: machine = "RISC-V 64 bit"; break;
    case 0x5128: machine = "RISC-V 128 bit"; break;
    case 0x8664: machine = "x64"; break;
    case 0xaa64: machine = "ARM64 Little Endian"; break;
    default: machine = "Unknown"; break;
  }

  printf("           Machine: 0x%04x (%s)\n", file_header->Machine, machine);
  printf("  NumberOfSections: %d\n", file_header->NumberOfSections);
  printf("     TimeDateStamp: %s", ctime((time_t *)&file_header->TimeDateStamp));
  printf("PointerToSymbolTbl: %d\n", file_header->PointerToSymbolTable);
  printf("   NumberOfSymbols: %d\n", file_header->NumberOfSymbols);
  printf(" SizeOfOptionalHdr: %d\n", file_header->SizeOfOptionalHeader);
  printf("   Characteristics: 0x%04x", file_header->Characteristics);
  if ((file_header->Characteristics & 0x0001) != 0)
  {
    printf(" (Relocations Stripped)");
  }

  if ((file_header->Characteristics & 0x0002) != 0)
  {
    printf(" (Executable Image)");
  }

  if ((file_header->Characteristics & 0x0004) != 0)
  {
    printf(" (Line Numbers Stripped)");
  }

  if ((file_header->Characteristics & 0x0008) != 0)
  {
    printf(" (File Local Symbols Stripped)");
  }

  if ((file_header->Characteristics & 0x0010) != 0)
  {
    printf(" (Agressively Trim Working Set)");
  }

  if ((file_header->Characteristics & 0x0020) != 0)
  {
    printf(" (Large Address Aware)");
  }

  if ((file_header->Characteristics & 0x0080) != 0)
  {
    printf(" (File Bytes Reversed)");
  }

  if ((file_header->Characteristics & 0x0100) != 0)
  {
    printf(" (32 Bit Machine)");
  }

  if ((file_header->Characteristics & 0x0400) != 0)
  {
    printf(" (File Removable, Run From Swap)");
  }

  if ((file_header->Characteristics & 0x0800) != 0)
  {
    printf(" (File Networked, Run From Swap)");
  }

  if ((file_header->Characteristics & 0x1000) != 0)
  {
    printf(" (File System Image)");
  }

  if ((file_header->Characteristics & 0x2000) != 0)
  {
    printf(" (Dynamic Link Library)");
  }

  if ((file_header->Characteristics & 0x4000) != 0)
  {
    printf(" (Uniprocessor Only)");
  }

  if ((file_header->Characteristics & 0x8000) != 0)
  {
    printf(" (File Bytes Reversed Hi)");
  }

  printf("\n\n");

  return 0;
}

