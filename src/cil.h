/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _CIL_H
#define _CIL_H

#include <stdint.h>

#include "section_header.h"

struct _clr_header
{
  uint32_t header_size;
  uint16_t major_runtime_version;
  uint16_t minor_runtime_version;
  uint32_t meta_data_directory_address;
  uint32_t meta_data_directory_size;
  uint32_t flags;
  uint32_t entry_point_token;
  uint32_t resources_directory_address;
  uint32_t resources_directory_size;
  uint32_t strong_name_signature_address;
  uint32_t strong_name_signature_size;
  uint32_t v_table_fixups_address;
  uint32_t v_table_fixups_size;
  uint32_t export_address_table_jump_address;
  uint32_t export_address_table_jump_size;
  uint32_t managed_native_header_address;
  uint32_t managed_native_header_size;
};

struct _code_section
{
  uint16_t flags;
  uint16_t max_stack;
  uint32_t code_size;
  uint32_t local_var_sig;
  uint8_t code[];
};

void disasm_cil(uint8_t *buffer, int len);
int read_clr_header(FILE *in, struct _clr_header *clr_header, int address, int size, struct section_header_t *section_header);
void print_clr_header(struct _clr_header *clr_header);

#endif

