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

#include "debug_directory.h"
#include "fileio.h"

int debug_directory_read(struct debug_directory_t *debug_directory, FILE *in)
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

int debug_directory_print(struct debug_directory_t *debug_directory)
{
  char *debug_types[] =
  {
    "Unknown",
    "COFF",
    "CODEVIEW",
    "FPO",
    "MISC",
    "EXCEPTION",
    "FIXUP",
    "OMAP_TO_SRC",
    "OMAP_FROM_SRC",
    "BORLAND",
    "RESERVED10"
  };

  printf("-- Debug Directory --\n");
  printf("   Characteristics: %d\n", debug_directory->Characteristics);
  printf("     TimeDateStamp: %s", ctime((time_t *)&debug_directory->TimeDateStamp));
  printf("      MajorVersion: %d\n", debug_directory->MajorVersion);
  printf("      MinorVersion: %d\n", debug_directory->MinorVersion);
  printf("              Type: %d (", debug_directory->Type);

  if (debug_directory->Type < 11)
  {
    printf("%s", debug_types[debug_directory->Type]);
  }
    else
  {
    printf("???");
  }

  printf(")\n");
  printf("        SizeOfData: %d\n", debug_directory->SizeOfData);
  printf("  AddressOfRawData: %d\n", debug_directory->AddressOfRawData);
  printf("  PointerToRawData: %d\n", debug_directory->PointerToRawData);
  printf("\n");

  return 0;
}

