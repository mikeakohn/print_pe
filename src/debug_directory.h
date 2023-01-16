/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef DEBUG_DIRECTORY_H
#define DEBUG_DIRECTORY_H

#include <stdint.h>

#include "section_header.h"

struct debug_directory_t
{
  uint32_t characteristics;
  uint32_t time_date_stamp;
  uint16_t major_version;
  uint16_t minor_version;
  uint32_t type;
  uint32_t size_of_data;
  uint32_t address_of_raw_data;
  uint32_t pointer_to_raw_data;
};

int debug_directory_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header);

#endif

