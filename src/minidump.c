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
#include "minidump_stream_type.h"

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

int read_minidump_dir(struct minidump_dir_t *minidump_dir, FILE *in)
{
  minidump_dir->stream_type = read_uint32(in);
  minidump_dir->len_data = read_uint32(in);
  minidump_dir->ofs_data = read_uint32(in);

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

void print_minidump_dir(struct minidump_dir_t *minidump_dir, int index)
{
  printf(" -------- MiniDump dir %d -------\n", index);
  printf("stream_type: %d (%s)\n",
    minidump_dir->stream_type,
    get_minidump_stream_type(minidump_dir->stream_type));
  printf("   len_data: %d\n", minidump_dir->len_data);
  printf("   ofs_data: %d\n", minidump_dir->ofs_data);
}

void print_minidump_system_info(FILE *in)
{
  int n;

  printf("\n");
  printf("         cpu_arch: %d\n", read_uint16(in));
  printf("        cpu_level: %d\n", read_uint16(in));
  printf("     cpu_revision: %d\n", read_uint16(in));
  printf("         num_cpus: %d\n", read_uint8(in));
  printf("          os_type: %d\n", read_uint8(in));
  printf("     os_ver_major: %d\n", read_uint32(in));
  printf("     os_ver_minor: %d\n", read_uint32(in));
  printf("         os_build: %d\n", read_uint32(in));
  printf("      os_platform: %d\n", read_uint32(in));

  uint32_t ofs_service_pack = read_uint32(in);
  long marker = ftell(in);
  fseek(in, ofs_service_pack, SEEK_SET);

  uint32_t length = read_uint32(in);

  for (n = 0; n < length; n++)
  {
    putchar(getc(in));
  }

  printf(" ofs_service_pack: ");
  printf("\n");

  fseek(in, marker, SEEK_SET);

  printf("    os_suite_mask: %d\n", read_uint16(in));
  printf("        reserved2: %d\n", read_uint16(in));
}

