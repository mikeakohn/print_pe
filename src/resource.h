/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef RESOURCE_H
#define RESOURCE_H

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

struct fixed_file_info_t
{
  uint32_t Signature;
  uint32_t StrucVersion;
  uint32_t FileVersionMS;
  uint32_t FileVersionLS;
  uint32_t ProductVersionMS;
  uint32_t ProductVersionLS;
  uint32_t FileFlagsMask;
  uint32_t FileFlags;
  uint32_t FileOS;
  uint32_t FileType;
  uint32_t FileSubtype;
  uint32_t FileDateMS;
  uint32_t FileDateLS;
};

int parse_resource_dir(
  FILE *in,
  struct section_header_t *section_header,
  int offset,
  int level,
  int res_type,
  int dump);

int read_resource_data(
  FILE *in,
  int addr,
  struct resource_data_t *resource_data,
  int offset);

int read_resource_dir(FILE *in, int addr, struct resource_dir_t *resource_dir);

int read_resource_dir_entry(
  FILE *in,
  int addr,
  struct resource_dir_entry_t *resource_dir_entry,
  int offset);

int read_fixed_file_info(FILE *in, struct fixed_file_info_t *fixed_file_info);

int print_resource_data(struct resource_data_t *resource_data, int level);
int print_resource_type(int id, int level);
int print_resource_dir(struct resource_dir_t *resource_dir, int level);
int print_resource_dir_entry(struct resource_dir_entry_t *resource_dir_entry, int level);
int print_fixed_file_info(struct fixed_file_info_t *fixed_file_info);

#endif

