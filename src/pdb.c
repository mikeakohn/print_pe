/*

print_pdb / print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "fileio.h"
#include "pdb.h"

static int calc_block_count(uint32_t size)
{
  size = (size + 4095) & 0xfffff000;

  return size / 4096;
}

static uint32_t *get_blocks(struct pdb_dir_t *pdb_dir, int index)
{
  int n, ptr = 0;

  for (n = 0; n < index; n++)
  {
    ptr += calc_block_count(pdb_dir->stream_sizes[n]);
  }

  return &pdb_dir->stream_blocks[ptr];
}

static void read_data(
  struct pdb_dir_t *pdb_dir,
  int index,
  int block_size,
  FILE *in)
{
  long marker = ftell(in);
  uint32_t *blocks = get_blocks(pdb_dir, index);
  int block_count = calc_block_count(pdb_dir->stream_sizes[index]);
  int n, offset = 0, length;

  for (n = 0; n < block_count; n++)
  {
    fseek(in, *blocks * block_size, SEEK_SET);
    length = fread(pdb_dir->heap + offset, 4096, 1, in);

    if (length == 0) { printf("Error: fread() 0 bytes.\n"); }

    offset += 4096;
    blocks++;
  }

  fseek(in, marker, SEEK_SET);
}

static uint16_t get_uint16(void *buffer)
{
  uint8_t *data = (uint8_t *)buffer;

  return data[0] | (data[1] << 8);
}

static int16_t get_int16(void *buffer)
{
  uint8_t *data = (uint8_t *)buffer;

  return data[0] | (data[1] << 8);
}

static uint32_t get_uint32(void *buffer)
{
  uint8_t *data = (uint8_t *)buffer;

  return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

static int32_t get_int32(void *buffer)
{
  uint8_t *data = (uint8_t *)buffer;

  return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

#if 0
static void load_hash(
  struct hash_t *hash,
  uint32_t size,
  uint32_t capacity,
  FILE *in)
{
  uint32_t heap_offset =
     sizeof(struct hash_t) +
    (sizeof(void *) * capacity);

  hash->size = size;
  hash->capacity = capacity;
  hash->present_bit_vector = read_uint32(in);
  hash->deleted_bit_vector = read_uint32(in);

  int n;

  for (n = 0; n < size; n++)
  {
    uint32_t key = read_uint32(in);
    uint32_t value = read_uint32(in);

    printf("read hash entry: %d %d\n", key, value);
  }
}

static void dump_hash(struct hash_t *hash)
{
  int n;

  printf(" hash: present_bit_vector=%d, deleted_bit_vector=%d\n",
    hash->present_bit_vector,
    hash->deleted_bit_vector);

  for (n = 0; n < hash->capacity; n++)
  {
    struct hash_entry_t *entry = hash->hash_entry[n];

    if (entry == NULL) { continue; }

    printf(" %d:", n);

    while (entry != NULL)
    {
      printf(" [%d->%d]", entry->key, entry->value);
      entry = entry->next;
    }
  }
}
#endif

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

  uint32_t heap_length = 0;

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

    // FIXME: For some reason a PDB used for testing this is reading in
    // a stream size of -1.
    if (pdb_dir->stream_sizes[n] == 0xffffffff)
    {
      pdb_dir->stream_sizes[n] = 0;
    }

    if (heap_length < pdb_dir->stream_sizes[n])
    {
      heap_length = pdb_dir->stream_sizes[n];
    }

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

  heap_length = (heap_length + 4095) & 0xfffff000;
  pdb_dir->heap = (uint8_t *)malloc(heap_length);

  fseek(in, marker, SEEK_SET);

  return 0;
}

int read_pdb_names(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in)
{
  const int index = pdb_dir->names_index;
  const int length = pdb_dir->stream_sizes[index];

  if (index == 0)
  {
    printf("Error: No /names stream found.\n");
    return -1;
  }

  read_data(pdb_dir, index, pdb_header->block_size, in);
  pdb_dir->names = (uint8_t *)malloc(length);
  memcpy(pdb_dir->names, pdb_dir->heap, length);

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
  printf("\n");
}

void print_pdb_dir(struct pdb_dir_t *pdb_dir)
{
  int ptr = 0;
  int n, i;

  printf(" -- PDB Directory --\n");
  printf("   stream_count: %d\n", pdb_dir->stream_count);
  printf("  stream_blocks:\n");

  for (n = 0; n < pdb_dir->stream_count; n++)
  {
    int count = calc_block_count(pdb_dir->stream_sizes[n]);

    printf("    %d: %6d {", n, pdb_dir->stream_sizes[n]);

    for (i = 0; i < count; i++)
    {
      printf(" %d", pdb_dir->stream_blocks[ptr++]);
    }

    printf(" }\n");
  }
}

void print_pdb_stream_info(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in)
{
  //int total_length = pdb_dir->stream_sizes[1];
  int i;

  read_data(pdb_dir, 1, pdb_header->block_size, in);

  uint8_t *data = pdb_dir->heap;

  printf(" -- PDB Info Stream (index 1) --\n");

  printf("     version: %d\n", get_uint32(data + 0));
  printf("   signature: %d\n", get_uint32(data + 4));
  printf("         age: %d\n", get_uint32(data + 8));
  printf("        guid:");

  for (i = 0; i < 16; i++)
  {
    printf(" %02x", data[i + 12]);
  }
  printf("\n\n");

  // This is the length of the names section.
  int length = get_uint32(data + 28);

  uint8_t *names = data + 32;
  uint8_t *hash = names + length;

  uint32_t hash_size = get_uint32(hash + 0);
  uint32_t hash_capacity = get_uint32(hash + 4);
  uint32_t vector_words = get_uint32(hash + 8);

  printf("         hash_size: %d\n", hash_size);
  printf("     hash_capacity: %d\n", hash_capacity);
  printf("  bit_vector_count: %d\n", vector_words);
  printf("\n");

  // FIXME: According to some docs, the full hash is supposed to be in
  // the file. So the for loop below should be hash_capacity in length
  // and buckets are ignored by bits in the vector_mask. In the test file
  // though, doing that ends up with 11 words for a capacity of 6 (should
  // be 12 words in the file. Or maybe it's 10 words because the deleted
  // buckets is it's own word (as it's implemented here). This seems to work
  // though.

  hash += 12;

  //const uint32_t *vector_bits = (uint32_t *)hash;
  hash += vector_words * 4 * 2;

  for (i = 0; i < hash_size; i++)
  {
    //if ((vector_bits[ptr] & bit_mask) != 0)
    {
      uint32_t key = get_uint32(hash + 0);
      uint32_t value = get_uint32(hash + 4);
      const char *name = (char *)names + key;

      printf("%d: index=%-2d %s (offset=%d)\n", i, value, name, key);

      if (strcmp("/names", name) == 0)
      {
        pdb_dir->names_index = value;
      }
    }

    hash += 8;
  };

  printf("\n");
}

void print_pdb_tpi_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in,
  int index)
{
  struct pdb_tpi_header_t tpi;

  read_data(pdb_dir, index, pdb_header->block_size, in);

  uint8_t *data = pdb_dir->heap;

  tpi.version = get_uint32(data + 0);
  tpi.header_size = get_uint32(data + 4);
  tpi.type_index_begin = get_uint32(data + 8);
  tpi.type_index_end = get_uint32(data + 12);
  tpi.type_record_bytes = get_uint32(data + 16);
  tpi.hash_stream_index = get_uint16(data + 20);
  tpi.hash_aux_stream_index = get_uint16(data + 22);
  tpi.hash_key_size = get_uint32(data + 24);
  tpi.num_hash_buckets = get_uint32(data + 28);
  tpi.hash_value_buffer_offset = get_int32(data + 32);
  tpi.hash_value_buffer_length = get_uint32(data + 36);
  tpi.index_offset_buffer_offset = get_int32(data + 40);
  tpi.index_offset_buffer_length = get_uint32(data + 44);
  tpi.hash_adj_buffer_offset = get_int32(data + 48);
  tpi.hash_adj_buffer_length = get_uint32(data + 52);

  printf(" -- %s Stream --\n", index == 2 ? "TPI" : "IPI");
  printf("                   version: %d\n", tpi.version);
  printf("               header_size: %d\n", tpi.header_size);
  printf("          type_index_begin: %d\n", tpi.type_index_begin);
  printf("            type_index_end: %d\n", tpi.type_index_end);
  printf("         type_record_bytes: %d\n", tpi.type_record_bytes);
  printf("         hash_stream_index: %d\n", tpi.hash_stream_index);
  printf("     hash_aux_stream_index: %d\n", tpi.hash_aux_stream_index);
  printf("             hash_key_size: %d\n", tpi.hash_key_size);
  printf("          num_hash_buckets: %d\n", tpi.num_hash_buckets);
  printf("  hash_value_buffer_offset: %d\n", tpi.hash_value_buffer_offset);
  printf("  hash_value_buffer_length: %d\n", tpi.hash_value_buffer_length);
  printf("index_offset_buffer_offset: %d\n", tpi.index_offset_buffer_offset);
  printf("index_offset_buffer_length: %d\n", tpi.index_offset_buffer_length);
  printf("    hash_adj_buffer_offset: %d\n", tpi.hash_adj_buffer_offset);
  printf("    hash_adj_buffer_length: %d\n", tpi.hash_adj_buffer_length);
  printf("\n");

  int offset = 0;
  int id, unknown, n;
  uint8_t *next = data + tpi.header_size;

  while (offset < tpi.type_record_bytes)
  {
    struct type_record_t *type_record = (struct type_record_t *)next;
    int total_length = type_record->length + 2;

    switch (type_record->type)
    {
      case 0x1001:
        printf(" %d 0x%04x MODIFIER\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1002:
        printf(" %d 0x%04x POINTER\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1008:
        printf(" %d 0x%04x PROCEDURE\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1009:
        printf(" %d 0x%04x MFUNCTION\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1201:
        printf(" %d 0x%04x ARGLIST\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1203:
        printf(" %d 0x%04x FIELDLIST\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1206:
        printf(" %d 0x%04x METHODLIST\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1503:
        printf(" %d 0x%04x ARRAY\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1504:
        printf(" %d 0x%04x CLASS %s\n",
          type_record->length,
          type_record->type,
          next + 22);
        break;
      case 0x1505:
        printf(" %d 0x%04x STRUCTURE %s\n",
          type_record->length,
          type_record->type,
          next + 22);
        //printf("offset=0x%04x\n", offset + 56);
        break;
      case 0x1507:
        printf(" %d 0x%04x ENUM\n",
          type_record->length,
          type_record->type);
        break;
      case 0x1601:
        id = get_uint32(next + 4);
        unknown = get_uint32(next + 8);
        printf(" %d 0x%04x FUNC_ID [%d][0x%08x]\n   %s\n",
          type_record->length,
          type_record->type,
          id,
          unknown,
          next + 12);
        break;
      case 0x1602:
        id = get_uint32(next + 4);
        unknown = get_uint32(next + 8);
        printf(" %d 0x%04x MFUNC_ID [%d][0x%08x]\n   %s\n",
          type_record->length,
          type_record->type,
          id,
          unknown,
          next + 12);
        break;
      case 0x1605:
        id = get_uint32(next + 4);
        printf(" %d 0x%04x STRING_ID [%d]\n   %s\n",
          type_record->length,
          type_record->type,
          id,
          next + 8);
        break;
      case 0x1607:
        printf(" %d 0x%04x UDT_MOD_SRC_LINE\n",
          type_record->length,
          type_record->type);
        printf("  {");
        for (n = 0; n < type_record->length - 2; n++)
        {
          printf(" %02x", next[n + 4]);
        }
        printf(" }\n");
        break;
      default:
        printf(" %d 0x%04x\n", type_record->length, type_record->type);
        // printf("offset=0x%04x\n", offset + 56);
        break;
    }

    next += total_length;
    offset += total_length;
  }
}

void read_pdb_dbi_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  struct pdb_dbi_t *pdb_dbi,
  FILE *in)
{
  read_data(pdb_dir, 3, pdb_header->block_size, in);

  uint8_t *data = pdb_dir->heap;

  pdb_dbi->version_signature = get_int32(data + 0);
  pdb_dbi->version_header = get_uint32(data + 4);
  pdb_dbi->age = get_uint32(data + 8);
  pdb_dbi->global_stream_index = get_int16(data + 12);
  pdb_dbi->build_number = get_int16(data + 14);
  pdb_dbi->public_stream_index = get_int16(data + 16);
  pdb_dbi->pdb_dll_version = get_int16(data + 18);
  pdb_dbi->symbol_record_stream = get_int16(data + 20);
  pdb_dbi->pdb_dll_rbld = get_int16(data + 22);
  pdb_dbi->mod_info_size = get_int32(data + 24);
  pdb_dbi->section_contribution_size = get_int32(data + 28);
  pdb_dbi->section_map_size = get_int32(data + 32);
  pdb_dbi->source_info_size = get_int32(data + 36);
  pdb_dbi->type_server_size = get_int32(data + 40);
  pdb_dbi->mfc_type_server_index = get_uint32(data + 44);
  pdb_dbi->optional_debug_header_size = get_int32(data + 48);
  pdb_dbi->ec_substream_size = get_int32(data + 52);
  pdb_dbi->flags = get_int16(data + 56);
  pdb_dbi->machine = get_uint16(data + 58);
  pdb_dbi->padding = get_uint32(data + 60);
}

void print_pdb_dbi_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  struct pdb_dbi_t *pdb_dbi,
  FILE *in)
{
  if (pdb_dbi->version_signature == 0)
  {
    read_pdb_dbi_stream(pdb_dir, pdb_header, pdb_dbi, in);
  }

  uint8_t *data = pdb_dir->heap;

#if 0
  read_data(pdb_dir, 3, pdb_header->block_size, in);

  uint8_t *data = pdb_dir->heap;

  pdb_dbi->version_signature = get_int32(data + 0);
  pdb_dbi->version_header = get_uint32(data + 4);
  pdb_dbi->age = get_uint32(data + 8);
  pdb_dbi->global_stream_index = get_int16(data + 12);
  pdb_dbi->build_number = get_int16(data + 14);
  pdb_dbi->public_stream_index = get_int16(data + 16);
  pdb_dbi->pdb_dll_version = get_int16(data + 18);
  pdb_dbi->symbol_record_stream = get_int16(data + 20);
  pdb_dbi->pdb_dll_rbld = get_int16(data + 22);
  pdb_dbi->mod_info_size = get_int32(data + 24);
  pdb_dbi->section_contribution_size = get_int32(data + 28);
  pdb_dbi->section_map_size = get_int32(data + 32);
  pdb_dbi->source_info_size = get_int32(data + 36);
  pdb_dbi->type_server_size = get_int32(data + 40);
  pdb_dbi->mfc_type_server_index = get_uint32(data + 44);
  pdb_dbi->optional_debug_header_size = get_int32(data + 48);
  pdb_dbi->ec_substream_size = get_int32(data + 52);
  pdb_dbi->flags = get_int16(data + 56);
  pdb_dbi->machine = get_uint16(data + 58);
  pdb_dbi->padding = get_uint32(data + 60);
#endif

  printf(" -- DBI Stream --\n");
  printf("         version_signature: %d\n", pdb_dbi->version_signature);
  printf("            version_header: %d\n", pdb_dbi->version_header);
  printf("                       age: %d\n", pdb_dbi->age);
  printf("       global_stream_index: %d\n", pdb_dbi->global_stream_index);
  printf("              build_number: %d\n", pdb_dbi->build_number);
  printf("       public_stream_index: %d\n", pdb_dbi->public_stream_index);
  printf("           pdb_dll_version: %d\n", pdb_dbi->pdb_dll_version);
  printf("      symbol_record_stream: %d\n", pdb_dbi->symbol_record_stream);
  printf("              pdb_dll_rbld: %d\n", pdb_dbi->pdb_dll_rbld);
  printf("             mod_info_size: %d\n", pdb_dbi->mod_info_size);
  printf(" section_contribution_size: %d\n", pdb_dbi->section_contribution_size);
  printf("          section_map_size: %d\n", pdb_dbi->section_map_size);
  printf("          source_info_size: %d\n", pdb_dbi->source_info_size);
  printf("          type_server_size: %d\n", pdb_dbi->type_server_size);
  printf("     mfc_type_server_index: %d\n", pdb_dbi->mfc_type_server_index);
  printf("optional_debug_header_size: %d\n", pdb_dbi->optional_debug_header_size);
  printf("         ec_substream_size: %d\n", pdb_dbi->ec_substream_size);
  printf("                     flags: %d\n", pdb_dbi->flags);
  printf("                   machine: 0x%04x\n", pdb_dbi->machine);
  printf("                   padding: %d\n", pdb_dbi->padding);

  uint8_t *next = data + 64;
  int count = 0;
  uint32_t bytes = 0;
  int i;

  while (bytes < pdb_dbi->mod_info_size)
  {
    printf("       --- module %d ---\n", count);
    printf("            reserved_1: %d\n", get_uint32(next + 0));
    printf("               section: %d\n", get_int16(next + 4));
    printf("             padding_1: %d\n", get_uint16(next + 6));
    printf("                offset: %d\n", get_int32(next + 8));
    printf("                  size: %d\n", get_int32(next + 12));
    printf("       characteristics: 0x%08x\n", get_uint32(next + 16));
    printf("          module_index: %d\n", get_int16(next + 20));
    printf("             padding_2: %d\n", get_uint16(next + 22));
    printf("              data_crc: %08x\n", get_uint32(next + 24));
    printf("             reloc_crc: %08x\n", get_uint32(next + 28));
    printf("                 flags: %d\n", get_uint16(next + 32));
    printf("  module_symbol_stream: %d\n", get_uint16(next + 34));
    printf("      symbol_byte_size: %d\n", get_uint32(next + 36));
    printf("         c11_byte_size: %d\n", get_uint32(next + 40));
    printf("         c13_byte_size: %d\n", get_uint32(next + 44));
    printf("     source_file_count: %d\n", get_uint16(next + 48));
    printf("               padding: %d\n", get_uint16(next + 50));
    printf("            reserved_2: %d\n", get_uint32(next + 52));
    printf("source_file_name_index: %d\n", get_uint32(next + 56));
    printf("   pdb_file_path_index: %d\n", get_uint32(next + 60));
    printf("           module_name: ");

    for (i = 64; next[i] != 0; i++)
    {
      printf("%c", next[i]);
    }
    printf("\n");

    printf("      object_file_name: ");
    i++;
    for (;next[i] != 0; i++)
    {
      printf("%c", next[i]);
    }
    printf("\n");
    i++;

    i = (i + 3) & ~3;

    bytes += i;
    next += i;
    count++;
  }

  uint32_t version = get_uint32(next + 0);

  printf("    -- section contriubtion version: %08x\n", version);

  int length = 28;
  count = 0;
  bytes = 4;
  next += 4;

  if (version == 0xeffe0000 + 20140516) { length += 4; }

  while (bytes < pdb_dbi->section_contribution_size)
  {
    printf("       --- section contribution %d ---\n", count);
    printf("               section: %d\n", get_int16(next + 0));
    printf("             padding_1: %d\n", get_uint16(next + 2));
    printf("                offset: %d\n", get_int32(next + 4));
    printf("                  size: %d\n", get_int32(next + 8));
    printf("       characteristics: 0x%08x\n", get_uint32(next + 12));
    printf("          module_index: %d\n", get_uint16(next + 16));
    printf("             padding_2: %d\n", get_uint16(next + 18));
    printf("              data_crc: 0x%08x\n", get_uint32(next + 20));
    printf("             reloc_crc: 0x%08x\n", get_uint32(next + 24));

    if (version == 0xeffe0000 + 20140516)
    {
      printf("            isect_coff: %d\n", get_uint32(next + 28));
    }

    bytes += length;
    next += length;
    count++;
  }

  printf("    -- section map count=%d log_count=%d\n",
    get_uint16(next + 0),
    get_uint16(next + 2));

  count = 0;
  bytes = 4;
  next += 4;

  while (bytes < pdb_dbi->section_map_size)
  {
    uint16_t flags = get_uint16(next + 0);
    char temp[80];

    sprintf(temp, "%c%c%c %s %c%c%c",
      (flags & 1) != 0 ? 'r' : '-',
      (flags & 2) != 0 ? 'w' : '-',
      (flags & 4) != 0 ? 'e' : '-',
      (flags & 8) != 0 ? "32" : "-",
      (flags & 256) != 0 ? 'S' : '-',
      (flags & 512) != 0 ? 'A' : '-',
      (flags & 1024) != 0 ? 'G' : '-');

    printf("       --- section map entry %d ---\n", count);
    printf("                 flags: 0x%04x %s\n", flags, temp);
    printf("                   ovl: %d\n", get_uint16(next + 2));
    printf("                 group: %d\n", get_uint16(next + 4));
    printf("                 frame: %d\n", get_uint16(next + 6));
    printf("          section_name: %d\n", get_int16(next + 8));
    printf("            class_name: %d\n", get_int16(next + 10));
    printf("                offset: %d\n", get_uint32(next + 12));
    printf("        section_length: %d\n", get_uint32(next + 16));

    bytes += 20;
    next += 20;
    count++;
  }

  uint8_t *save = next;
  int module_count = get_uint16(next + 0);
  int source_file_count = get_uint16(next + 2);

  printf("    -- file_info modules=%d source_files=%d\n",
    module_count, source_file_count);

  bytes = 4;
  int offset = bytes + (module_count * 2);

  for (i = 0; i < module_count; i++)
  {
    printf("   module: indices=%d file_counts=%d\n",
      get_uint16(next + bytes), get_uint16(next + offset)); 
    bytes += 2;
    offset += 2;
  }

  next += module_count * 4;
  uint8_t *names_buffer = save + (module_count * 4) + (source_file_count * 4) + 4;

  for (i = 0; i < source_file_count; i++)
  {
    offset = get_uint32(next);
    printf("    %04x: %s\n", offset, names_buffer + offset);
    next += 4;
  }

  next = save + pdb_dbi->source_info_size;

  // FIXME: Skiping Type Server Substream.
  next += pdb_dbi->type_server_size;

  // FIXME: Skiping EC Substream.
  next += pdb_dbi->ec_substream_size;

  printf("       --- optional_debug ---\n");

  if (pdb_dbi->optional_debug_header_size >= 22)
  {
    printf("                     fpo_data: %d\n", get_int16(next + 0));
    printf("               exception data: %d\n", get_int16(next + 2));
    printf("                   fixup_data: %d\n", get_int16(next + 4));
    printf("             omap_to_src_data: %d\n", get_int16(next + 6));
    printf("           omap_from_src_data: %d\n", get_int16(next + 8));
    printf("          section_header_data: %d\n", get_int16(next + 10));
    printf("                token_rid_map: %d\n", get_int16(next + 12));
    printf("                        xdata: %d\n", get_int16(next + 14));
    printf("                        pdata: %d\n", get_int16(next + 16));
    printf("                 new_fpo_data: %d\n", get_int16(next + 18));
    printf(" original_section_header_data: %d\n", get_int16(next + 20));
    printf("\n");
  }
    else
  {
    printf("section size is only %d?\n", pdb_dbi->optional_debug_header_size);
  }

  printf("\n\n");
}

void print_pdb_symbols(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  struct pdb_dbi_t *pdb_dbi,
  FILE *in)
{
  if (pdb_dbi->version_signature == 0)
  {
    read_pdb_dbi_stream(pdb_dir, pdb_header, pdb_dbi, in);
  }

  read_data(pdb_dir, pdb_dbi->symbol_record_stream, pdb_header->block_size, in);

  struct symbol_record_t symbol_record;
  int length = pdb_dir->stream_sizes[pdb_dbi->symbol_record_stream];
  uint8_t *data = pdb_dir->heap;
  uint8_t *next = data;
  int n = 0;

  printf("  -- Symbols --\n");

  while (n < length)
  {
    int offset = 14;

    symbol_record.length = get_uint16(next + 0);
    symbol_record.type = get_uint16(next + 2);

    printf("       length: %d\n", symbol_record.length);

    switch (symbol_record.type)
    {
      case 0x110e:
        printf("  symbol_type: 0x%04x (PUB32)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
      case 0x1125:
        printf("  symbol_type: 0x%04x (PROCREF)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
      case 0x1107:
        printf("  symbol_type: 0x%04x (CONSTANT)\n", symbol_record.type);
        printf("       offset: 0x%04x\n", get_uint32(next + 4));
        printf("        index: %d\n", get_uint16(next + 8));
        offset = 10;
        break;
      case 0x1108:
        printf("  symbol_type: 0x%04x (UDT)\n", symbol_record.type);
        printf("       offset: 0x%04x\n", get_uint32(next + 4));
        offset = 8;
        break;
      case 0x110c:
        printf("  symbol_type: 0x%04x (LDATA32)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
      case 0x110d:
        printf("  symbol_type: 0x%04x (GDATA32)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
      case 0x1127:
        printf("  symbol_type: 0x%04x (LPROCREF)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
      default:
        printf("  symbol_type: 0x%04x (UNKNOWN)\n", symbol_record.type);
        printf("      section: %d\n", get_int32(next+ 4));
        printf("       offset: 0x%04x\n", get_uint32(next + 8));
        printf("        index: %d\n", get_uint16(next + 12));
        break;
    }

    printf("%s\n", next + offset);
    printf(" --------\n");

    int align = (symbol_record.length + 3) & ~3;

    next += align;
    n += align;
  }
}

void dump_pdb_stream(
  struct pdb_dir_t *pdb_dir,
  struct pdb_header_t *pdb_header,
  FILE *in,
  int index,
  const char *filename)
{
  read_data(pdb_dir, index, pdb_header->block_size, in);

  uint8_t *data = pdb_dir->heap;

  FILE *fp = fopen(filename, "wb");
  fwrite(data, pdb_dir->stream_sizes[index], 1, fp);
  fclose(fp);
}

