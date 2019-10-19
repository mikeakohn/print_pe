/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_RESOURCE_H
#define PE_RESOURCE_H

#include <stdint.h>

#include "pe.h"
#include "section_header.h"

struct resource_dir_t
{
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint16_t NumberOfNamedEntries;
  uint16_t NumberOfIdEntries;
};

struct resource_dir_entry_t
{
  uint32_t Name;
  uint32_t OffsetToData;
  char RealName[256];
};

struct resource_data_t
{
  uint32_t OffsetToData;
  uint32_t Size;
  uint32_t CodePage;
  uint32_t Reserved;
};

int parse_resource_dir(FILE *in, struct section_header_t *section_header, int offset, int level, int res_type);
int read_resource_data(FILE *in, int addr, struct resource_data_t *resource_data, int offset);
int read_resource_dir(FILE *in, int addr, struct resource_dir_t *resource_dir);
int read_resource_dir_entry(FILE *in, int addr, struct resource_dir_entry_t *resource_dir_entry, int offset);
int print_resource_data(struct resource_data_t *resource_data, int level);
int print_resource_type(int id, int level);
int print_resource_dir(struct resource_dir_t *resource_dir, int level);
int print_resource_dir_entry(struct resource_dir_entry_t *resource_dir_entry, int level);

#endif

