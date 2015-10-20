/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_IMPORTS_H
#define PE_IMPORTS_H

#include <stdint.h>

#include "pe_section_headers.h"

struct import_dir_t
{
  uint32_t FunctionNameList;
  uint32_t TimeDateStamp;
  uint32_t ForwardChain;
  uint32_t ModuleName;
  uint32_t FunctionAddressList;
};

int print_imports(FILE *in, int addr, int size, struct section_header_t *section_header);

#endif

