/*

print_pdb / print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "fileio.h"
#include "pdb.h"

static int calc_block_count(uint32_t size)
{
  size = (size + 4095) & 0xfffff000;

  return size / 4096;
}

int read_pdb_header(struct pdb_header_t *pdb_header, FILE *in)
{
  uint8_t magic_2[] = { 0x1a, 0x44, 0x53, 0x00, 0x00, 0x00 };
  int n;

  for (n = 0; n < sizeof(pdb_header->magic); n++)
  {
    pdb_header->magic[n] = getc(in);

    if (pdb_header->magic[n] == '\r') { pdb_header->magic[n] = 0; }
    if (pdb_header->magic[n] == '\n') { break; }
  }

  if (n == sizeof(pdb_header->magic)) { return -1; }

  for (n = 0; n < 6; n++)
  {
    pdb_header->magic_2[n] = getc(in);
    if (pdb_header->magic_2[n] != magic_2[n]) { return -1; }
  }

  pdb_header->block_size = read_uint32(in);
  pdb_header->free_block_map = read_uint32(in);
  pdb_header->number_of_blocks = read_uint32(in);
  pdb_header->number_of_dir_bytes = read_uint32(in);
  pdb_header->reserved = read_uint32(in);
  pdb_header->block_map_address = read_uint32(in);

  return 0;
}

int read_pdb_dir(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in)
{
  long marker = ftell(in);
  long block_marker;
  uint32_t block_bytes;
  uint32_t block_address;
  uint32_t n;

  fseek(in, pdb_header->block_map_address * pdb_header->block_size, SEEK_SET);

  block_address = read_uint32(in);
  block_marker = ftell(in);
  fseek(in, block_address * pdb_header->block_size, SEEK_SET);

  pdb_dir->stream_count = read_uint32(in);

  block_bytes = pdb_header->block_size - 4;

  for (n = 0; n < pdb_dir->stream_count; n++)
  {
    if (block_bytes == 0)
    {
      fseek(in, block_marker, SEEK_SET);
      block_address = read_uint32(in);
      block_marker = ftell(in);
      fseek(in, block_address * pdb_header->block_size, SEEK_SET);
      block_bytes = pdb_header->block_size;
    }

    pdb_dir->stream_sizes[n] = read_uint32(in);

    block_bytes -= 4;
  }

  pdb_dir->stream_blocks = pdb_dir->stream_sizes + pdb_dir->stream_count;

  int ptr = 0, i;

  for (n = 0; n < pdb_dir->stream_count; n++)
  {
    if (block_bytes == 0)
    {
      fseek(in, block_marker, SEEK_SET);
      block_address = read_uint32(in);
      block_marker = ftell(in);
      fseek(in, block_address * pdb_header->block_size, SEEK_SET);
      block_bytes = pdb_header->block_size;
    }

    int count = calc_block_count(pdb_dir->stream_sizes[n]);

    for (i = 0; i < count; i++)
    {
      pdb_dir->stream_blocks[ptr++] = read_uint32(in);
    }

    block_bytes -= 4;
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

void print_pdb_header(struct pdb_header_t *pdb_header)
{
  printf(" -------- PDB header -------\n");
  printf("              magic: %s\n", pdb_header->magic);
  printf("             magic2: %02x %02x %02x %02x %02x %02x\n",
    pdb_header->magic_2[0],
    pdb_header->magic_2[1],
    pdb_header->magic_2[2],
    pdb_header->magic_2[3],
    pdb_header->magic_2[4],
    pdb_header->magic_2[5]);
  printf("         block_size: %d\n", pdb_header->block_size);
  printf("     free_block_map: %d\n", pdb_header->free_block_map);
  printf("         num_blocks: %d\n", pdb_header->number_of_blocks);
  printf("      num_dir_bytes: %d\n", pdb_header->number_of_dir_bytes);
  printf("           reserved: %d\n", pdb_header->reserved);
  printf("  block_map_address: %d\n", pdb_header->block_map_address);
}

void print_pdb_dir(struct pdb_dir_t *pdb_dir)
{
  int ptr = 0;
  int n, i;

  printf(" -- PDB Directory --\n");
  printf("  stream_count: %d\n", pdb_dir->stream_count);
  printf("  stream_sizes:\n");

  for (n = 0; n < pdb_dir->stream_count; n++)
  {
    printf("      %d\n", pdb_dir->stream_sizes[n]);
  }

  printf("  stream_blocks:\n");

  for (n = 0; n < pdb_dir->stream_count; n++)
  {
    int count = calc_block_count(pdb_dir->stream_sizes[n]);

    printf("  {");

    for (i = 0; i < count; i++)
    {
      printf(" %d", pdb_dir->stream_blocks[ptr++]);
    }

    printf(" }\n");
  }
}

