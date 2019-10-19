/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_DEBUG_H
#define PE_DEBUG_H

#include <stdint.h>

#include "section_header.h"

struct debug_directory_t
{
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Type;
  uint32_t SizeOfData;
  uint32_t AddressOfRawData;
  uint32_t PointerToRawData;
};

int debug_directory_read(struct debug_directory_t *debug_directory, FILE *in);
int debug_directory_print(struct debug_directory_t *debug_directory);
int debug_section_print(FILE *in, int addr, int size, struct section_header_t *section_header);

#endif

