/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_SECTION_HEADERS_H
#define PE_SECTION_HEADERS_H

#include <stdint.h>

struct section_header_t
{
  char name[9];
  union
  {
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
  } Misc;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
};

int read_section_header(FILE *in, struct section_header_t *section_header);
int print_section_header(struct section_header_t *section_header,int count);

#endif

