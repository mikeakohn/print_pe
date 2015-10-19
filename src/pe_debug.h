/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_DEBUG_H
#define PE_DEBUG_H

#include <stdint.h>

#include "pe_section_headers.h"

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

int read_debug_directory(FILE *in, struct debug_directory_t *debug_directory);
int print_debug_directory(struct debug_directory_t *debug_directory);
int print_debug_section(FILE *in, int addr, int size, struct section_header_t *section_header);

#endif

