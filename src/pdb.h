/*

print_pdb / print_pe - Copyright 2005-2020 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _PDB_H
#define _PDB_H

#include <stdint.h>

struct pdb_header_t
{
  char magic[80];
  char magic_2[6];
  uint32_t block_size;
  uint32_t free_block_map;
  uint32_t number_of_blocks;
  uint32_t number_of_dir_bytes;
  uint32_t reserved;
  uint32_t block_map_address;
};

struct pdb_dir_t
{
  uint32_t stream_count;
  uint32_t *stream_sizes;
  uint32_t *stream_blocks;
};

int read_pdb_header(struct pdb_header_t *pdb_header, FILE *in);
int read_pdb_dir(struct pdb_dir_t *pdb_dir, struct pdb_header_t *pdb_header, FILE *in);

void print_pdb_header(struct pdb_header_t *pdb_header);
void print_pdb_dir(struct pdb_dir_t *pdb_dir);
void print_pdb_stream_info(struct pdb_dir_t *pdb_dir, struct pdb_header_t *pdb_header, FILE *in);

#endif

