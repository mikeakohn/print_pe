/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef SECTION_HEADER_H
#define SECTION_HEADER_H

#include <stdint.h>

struct section_header_t
{
  char name[10];
  uint32_t VirtualSize;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
};

int section_header_read(struct section_header_t *section_header, FILE *in);
int section_header_print(struct section_header_t *section_header, int count);

#endif

