/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "pe.h"
#include "pe_debug.h"

int read_debug_directory(FILE *in, struct debug_directory_t *debug_directory)
{
  debug_directory->Characteristics = read_uint32(in);
  debug_directory->TimeDateStamp = read_uint32(in);
  debug_directory->MajorVersion = read_uint16(in);
  debug_directory->MinorVersion = read_uint16(in);
  debug_directory->Type = read_uint32(in);
  debug_directory->SizeOfData = read_uint32(in);
  debug_directory->AddressOfRawData = read_uint32(in);
  debug_directory->PointerToRawData = read_uint32(in);

  return 0;
}

int print_debug_directory(struct debug_directory_t *debug_directory)
{
  char *debug_types[] = {
    "Unknown", "COFF", "CODEVIEW", "FPO", "MISC", "EXCEPTION", "FIXUP",
    "OMAP_TO_SRC", "OMAP_FROM_SRC", "BORLAND", "RESERVED10"
  };

  printf("-- Debug Directory --\n");
  printf("   Characteristics: %d\n", debug_directory->Characteristics);
  printf("     TimeDateStamp: %s", ctime((time_t *)&debug_directory->TimeDateStamp));
  printf("      MajorVersion: %d\n", debug_directory->MajorVersion);
  printf("      MinorVersion: %d\n", debug_directory->MinorVersion);
  printf("              Type: %d (", debug_directory->Type);

  if (debug_directory->Type < 11)
  { printf("%s", debug_types[debug_directory->Type]); }
    else
  { printf("???"); }

  printf(")\n");
  printf("        SizeOfData: %d\n", debug_directory->SizeOfData);
  printf("  AddressOfRawData: %d\n", debug_directory->AddressOfRawData);
  printf("  PointerToRawData: %d\n", debug_directory->PointerToRawData);
  printf("\n");

  return 0;
}

int print_debug_section(FILE *in, int addr, int size, struct section_header_t *section_header)
{
  struct debug_directory_t debug_directory;
  int marker,t,p,r;

  marker = ftell(in);
  fseek(in,addr,SEEK_SET);

  t = 0;
  while (t < size)
  {
    read_debug_directory(in, &debug_directory);
    print_debug_directory(&debug_directory);

    if (debug_directory.Type == 2)
    {
      p = ftell(in);
      fseek(in, debug_directory.PointerToRawData, SEEK_SET);
      printf("Unknown Header: ");
      for (r = 0; r < 4; r++)
      {
        printf("%08x ", read_uint32(in));
      }
      printf("\n");
      printf("Debug Filename: ");

      while(1)
      {
        r = getc(in);
        if (r == 0) break;
        printf("%c", r);
      }
      printf("\n\n");
      fseek(in, p, SEEK_SET);
    }
    // rip_binary(in, "debug.bin", debug_directory.PointerToRawData, debug_directory.SizeOfData);
    t = t + 28;
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}


