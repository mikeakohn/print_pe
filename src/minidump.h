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

int read_minidump_header(struct minidump_header_t *minidump_header, FILE *in);
int read_minidump_dir(struct minidump_dir_t *minidump_die, FILE *in);
void print_minidump_header(struct minidump_header_t *minidump_header);
void print_minidump_dir(struct minidump_dir_t *minidump_dir, int index);

#endif

