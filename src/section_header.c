/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "section_header.h"

int section_header_read(struct section_header_t *section_header, FILE *in)
{
  memset(section_header->name, 0, sizeof(section_header->name));
  read_chars(in, section_header->name, 8);
  section_header->VirtualSize = read_uint32(in);
  section_header->VirtualAddress = read_uint32(in);
  section_header->SizeOfRawData = read_uint32(in);
  section_header->PointerToRawData = read_uint32(in);
  section_header->PointerToRelocations = read_uint32(in);
  section_header->PointerToLinenumbers = read_uint32(in);
  section_header->NumberOfRelocations = read_uint16(in);
  section_header->NumberOfLinenumbers = read_uint16(in);
  section_header->Characteristics = read_uint32(in);

  return 0;
}

int section_header_print(struct section_header_t *section_header, int count)
{
  printf("---------------------------------------------\n");
  printf("Section Header %d\n",count);
  printf("---------------------------------------------\n");
  printf("      Section Name: %s\n",section_header->name);
  printf("       VirtualSize: %d\n",section_header->VirtualSize);
  printf("    VirtualAddress: 0x%04x\n",section_header->VirtualAddress);
  printf("     SizeOfRawData: %d\n",section_header->SizeOfRawData);
  printf("  PointerToRawData: %d\n",section_header->PointerToRawData);
  printf("  PtrToRelocations: %d\n",section_header->PointerToRelocations);
  printf("  PtrToLinenumbers: %d\n",section_header->PointerToLinenumbers);
  printf("  NumOfRelocations: %d\n",section_header->NumberOfRelocations);
  printf("  NumOfLinenumbers: %d\n",section_header->NumberOfLinenumbers);
  printf("   Characteristics: 0x%x\n",section_header->Characteristics);
  printf("\n");

  return 0;
}

