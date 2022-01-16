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

// The TPI and IPI headers are the same.
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

struct pdb_dbi_t
{
  int32_t  version_signature;
  uint32_t version_header;
  uint32_t age;
  int16_t  global_stream_index;
  uint16_t build_number;
  int16_t  public_stream_index;
  uint16_t pdb_dll_version;
  uint16_t symbol_record_stream;
  uint16_t pdb_dll_rbld;
  int32_t  mod_info_size;
  int32_t  section_contribution_size;
  int32_t  section_map_size;
  int32_t  source_info_size;
  int32_t  type_server_size;
  uint32_t mfc_type_server_index;
  int32_t  optional_debug_header_size;
  int32_t  ec_substream_size;
  uint16_t flags;
  uint16_t machine;
  uint32_t padding;
};

struct section_contrib_entry_t
{
  uint16_t section;
  uint16_t padding_1;
  int32_t offset;
  int32_t size;
  uint32_t characteristics;
  uint16_t module_index;
  uint16_t padding_2;
  uint32_t data_crc;
  uint32_t reloc_crc;
};

struct mod_info_t
{
  uint32_t reserved_1;
  struct section_contrib_entry_t entry;
  uint16_t flags;
  uint16_t module_symbol_stream;
  uint32_t symbol_byte_size;
  uint32_t c11_byte_size;
  uint32_t c13_byte_size;
  uint16_t source_file_count;
  uint16_t padding;
  uint32_t reserved_2;
  uint32_t source_file_name_index;
  uint32_t pdb_file_path_index;
};

struct type_record_t
{
  uint16_t length;
  uint16_t type;
};

struct hash_entry_t
{
  uint32_t key;
  uint32_t value;
  struct hash_entry_t *next;
};

struct hash_t
{
  uint32_t size;
  uint32_t capacity;
  uint32_t present_bit_vector;
  uint32_t deleted_bit_vector;
  struct hash_entry_t *hash_entry[];
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

void print_pdb_dbi_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  struct pdb_dbi_t *pdb_dbi,
  FILE *in);

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

