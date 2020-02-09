/*

print_minidump / print_pe - Copyright 2005-2020 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _MINIDUMP_H
#define _MINIDUMP_H

#include <stdint.h>

struct minidump_header_t
{
  char magic1[4];
  uint8_t magic2[2];
  uint16_t version;
  uint32_t num_streams;
  uint32_t ofs_streams;
  uint32_t checksum;
  uint32_t timestamp;
  uint64_t flags;
};

struct minidump_dir_t
{
  uint32_t stream_type;
  uint32_t len_data;
  uint32_t ofs_data;
};

struct minidump_system_info_t
{
  uint16_t cpu_arch;
  uint16_t cpu_level;
  uint16_t cpu_revision;
  uint8_t num_cpus;
  uint8_t os_type;
  uint32_t os_ver_major;
  uint32_t os_ver_minor;
  uint32_t os_build;
  uint32_t os_platform;
  uint32_t ofs_service_pack;
  uint16_t os_suite_mask;
  uint16_t reserved2;
};

struct minidump_location_desc_t
{
  uint32_t len_data;
  uint32_t ofs_data;
};

int read_minidump_header(struct minidump_header_t *minidump_header, FILE *in);
int read_minidump_dir(struct minidump_dir_t *minidump_dir, FILE *in);
int read_minidump_location_desc(struct minidump_location_desc_t *minidump_location_desc, FILE *in);
void print_minidump_header(struct minidump_header_t *minidump_header);
void print_minidump_dir(struct minidump_dir_t *minidump_dir, int index);

void print_minidump_thread_list(FILE *in);
void print_minidump_memory_list(FILE *in);
void print_minidump_module_list(FILE *in);
void print_minidump_system_info(FILE *in);
void print_minidump_misc_info(FILE *in);
void print_minidump_thread_info_list(FILE *in);

#endif

