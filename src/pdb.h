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

  uint8_t *heap;
};

struct pdb_tpi_header_t
{
  uint32_t version;
  uint32_t header_size;
  uint32_t type_index_begin;
  uint32_t type_index_end;
  uint32_t type_record_bytes;
  uint16_t hash_stream_index;
  uint16_t hash_aux_stream_index;
  uint32_t hash_key_size;
  uint32_t num_hash_buckets;
  int32_t  hash_value_buffer_offset;
  uint32_t hash_value_buffer_length;
  int32_t  index_offset_buffer_offset;
  uint32_t index_offset_buffer_length;
  int32_t  hash_adj_buffer_offset;
  uint32_t hash_adj_buffer_length;
};

struct type_record_t
{
  uint16_t length;
  uint16_t type;
};

int read_pdb_header(struct pdb_header_t *pdb_header, FILE *in);
int read_pdb_dir(struct pdb_dir_t *pdb_dir, struct pdb_header_t *pdb_header, FILE *in);

void print_pdb_header(struct pdb_header_t *pdb_header);
void print_pdb_dir(struct pdb_dir_t *pdb_dir);

void print_pdb_stream_info(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in);

void print_pdb_tpi_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in,
  int index);

void print_pdb_names(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in);

void print_pdb_public(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in);

void print_pdb_global(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in);

// For debugging.
void dump_pdb_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in,
  int index,
  const char *filename);

#endif

