/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef EXPORTS_H
#define EXPORTS_H

#include <stdint.h>

#include "section_header.h"

struct funct_t
{
  char funct_name[1024];
  unsigned int funct_retvalue;
  long funct_ptr;
  long file_offset;
};

struct export_dir_t
{
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Name;
  uint32_t Base;
  uint32_t NumberOfFunctions;
  uint32_t NumberOfNames;
  uint32_t AddressOfFunctions;
  uint32_t AddressOfNames;
  uint32_t AddressOfNameOrdinals;
};

int exports_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header,
  struct funct_t *funct);

#endif

