/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef IMPORTS_H
#define IMPORTS_H

#include <stdint.h>

#include "section_header.h"

struct import_dir_t
{
  uint32_t FunctionNameList;
  uint32_t TimeDateStamp;
  uint32_t ForwardChain;
  uint32_t ModuleName;
  uint32_t FunctionAddressList;
};

int imports_print(
  FILE *in,
  int addr,
  int size,
  int address_size,
  struct section_header_t *section_header);

int imports_find(
  FILE *in,
  int address,
  int size,
  int address_size,
  struct section_header_t *section_header,
  const char *search_name);

#endif

