/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef DEBUG_DIRECTORY_H
#define DEBUG_DIRECTORY_H

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

int debug_directory_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header);

#endif

