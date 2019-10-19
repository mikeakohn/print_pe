/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef FILE_HEADERS_H
#define FILE_HEADERS_H

#include <stdint.h>

struct file_header_t
{
  uint16_t Machine;
  uint16_t NumberOfSections;
  uint32_t TimeDateStamp;
  uint32_t PointerToSymbolTable;
  uint32_t NumberOfSymbols;
  uint16_t SizeOfOptionalHeader;
  uint16_t Characteristics;
};

int file_header_read(struct file_header_t *image_file_header, FILE *in);
int file_header_print(struct file_header_t *image_file_header);

#endif

