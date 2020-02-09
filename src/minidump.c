/*

print_minidump / print_pe - Copyright 2005-2020 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"
#include "minidump.h"

int read_minidump_header(struct minidump_header_t *minidump_header, FILE *in)
{
  read_chars(in, minidump_header->magic1, 4);
  read_chars_bin(in, minidump_header->magic2, 2);
  minidump_header->version = read_uint16(in);
  minidump_header->num_streams = read_uint32(in);
  minidump_header->ofs_streams = read_uint32(in);
  minidump_header->checksum = read_uint32(in);
  minidump_header->timestamp = read_uint32(in);
  minidump_header->flags = read_uint64(in);

  return 0;
}

void print_minidump_header(struct minidump_header_t *minidump_header)
{
  printf(" -------- MiniDump header -------\n");
  printf("     magic1: %.4s\n", minidump_header->magic1);
  printf(     "magic2: 0x%02x 0x%02x\n", minidump_header->magic2[0],
                                         minidump_header->magic2[1]);
  printf("    version: %d\n", minidump_header->version);
  printf("num_streams: %d\n", minidump_header->num_streams);
  printf("ofs_streams: %d\n", minidump_header->ofs_streams);
  printf("   checksum: %d\n", minidump_header->checksum);
  printf("  timestamp: %d\n", minidump_header->timestamp);
  printf("      flags: 0x%lx\n", minidump_header->flags);
}

