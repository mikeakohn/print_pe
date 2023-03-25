/*

print_lib / print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "fileio.h"
//#include "pdb.h"

struct file_header_t
{
  long offset;
  char name[16];
  char date[12];
  char user_id[6];
  char group_id[6];
  char mode[8];
  char size[10];
  char end[2];
};

struct coff_header_t
{
  long offset;
  uint16_t magic;
  uint16_t section_count;
  uint32_t timestamp;
  uint32_t symbol_table_pointer;
  uint32_t symbol_count;
  uint16_t optional_header_size;
  uint16_t flags;
};

struct coff_optional_header_t
{
  long offset;
  uint16_t magic;
  uint16_t version;
  uint32_t text_size;
  uint32_t data_size;
  uint32_t uninit_size;
  uint32_t entry_point;
  uint32_t text_start;
  uint32_t data_start;
};

struct coff_section_header_t
{
  long offset;
  char name[8];
  uint32_t physical_address;
  uint32_t virtual_address;
  uint32_t size;
  uint32_t file_offset_section_data;
  uint32_t file_offset_relocation_table;
  uint32_t file_offset_line_number;
  uint16_t relocation_count;
  uint16_t line_number_count;
  uint32_t flags;
};

struct coff_symbol_header_t
{
  long offset;
  uint16_t version;
  uint16_t machine;
  uint32_t timestamp;
  uint32_t size;
  uint16_t unknown_0;
  uint16_t unknown_1;
};

int read_file_header(FILE *in, struct file_header_t *file_header)
{
  // Align to 2 bytes if needed.
  while ((ftell(in) % 2) != 0) { getc(in); }

  file_header->offset = ftell(in);

  if (fread(file_header->name,     1, 16, in) != 16) { return -1; }
  if (fread(file_header->date,     1, 12, in) != 12) { return -1; }
  if (fread(file_header->user_id,  1,  6, in) !=  6) { return -1; }
  if (fread(file_header->group_id, 1,  6, in) !=  6) { return -1; }
  if (fread(file_header->mode,     1,  8, in) !=  8) { return -1; }
  if (fread(file_header->size,     1, 10, in) != 10) { return -1; }
  if (fread(file_header->end,      1,  2, in) !=  2) { return -1; }

  return 0;
}

int read_coff_header(FILE *in, struct coff_header_t *coff_header)
{
  coff_header->offset = ftell(in);

  coff_header->magic = read_uint16(in);
  coff_header->section_count = read_uint16(in);

  // It seems like if the magic number is 0 (and I guess section count
  // is 0xffff) that this isn't a COFF section.
  if (coff_header->magic == 0 && coff_header->section_count == 0xffff)
  {
    return -2;
  }

  coff_header->timestamp = read_uint32(in);
  coff_header->symbol_table_pointer = read_uint32(in);
  coff_header->symbol_count = read_uint32(in);
  coff_header->optional_header_size = read_uint16(in);
  coff_header->flags = read_uint16(in);

  return 0;
}

int read_coff_optional_header(
  FILE *in,
  struct coff_optional_header_t *optional_header)
{
  optional_header->offset = ftell(in);

  optional_header->magic = read_uint16(in);
  optional_header->version = read_uint16(in);
  optional_header->text_size = read_uint32(in);
  optional_header->data_size = read_uint32(in);
  optional_header->uninit_size = read_uint32(in);
  optional_header->entry_point = read_uint32(in);
  optional_header->text_start = read_uint32(in);
  optional_header->data_start = read_uint32(in);

  return 0;
}

int read_coff_section_header(
  FILE *in,
  struct coff_section_header_t *section_header)
{
  section_header->offset = ftell(in);

  if (fread(section_header->name, 1, 8, in) != 8) { return -1; }

  section_header->physical_address = read_uint32(in);
  section_header->virtual_address = read_uint32(in);
  section_header->size = read_uint32(in);
  section_header->file_offset_section_data = read_uint32(in);
  section_header->file_offset_relocation_table = read_uint32(in);
  section_header->file_offset_line_number = read_uint32(in);
  section_header->relocation_count = read_uint16(in);
  section_header->line_number_count = read_uint16(in);
  section_header->flags = read_uint32(in);

  return 0;
}

void print_file_header(struct file_header_t *file_header)
{
  printf(" --- file header 0x%06lx ---\n", file_header->offset);
  printf("        name: %.16s\n", file_header->name);
  printf("        date: %.12s\n", file_header->date);
  printf("     user_id: %.6s\n", file_header->user_id);
  printf("    group_id: %.6s\n", file_header->group_id);
  printf("        mode: %.8s\n", file_header->mode);
  printf("        size: %.10s\n", file_header->size);
  printf("         end: %.2s\n", file_header->end);
}

void print_coff_header(struct coff_header_t *coff_header)
{
  printf(" --- coff header 0x%06lx ---\n", coff_header->offset);
  printf("                magic: 0x%04x\n", coff_header->magic);
  printf("        section_count: %d\n", coff_header->section_count);
  printf("            timestamp: %d\n", coff_header->timestamp);
  printf(" symbol_table_pointer: 0x%04x\n", coff_header->symbol_table_pointer);
  printf("         symbol_count: %d\n", coff_header->symbol_count);
  printf(" optional_header_size: %d\n", coff_header->optional_header_size);
  printf("                flags: %08x\n", coff_header->flags);
  printf("\n");
}

void print_coff_optional_header(struct coff_optional_header_t *optional_header)
{
  printf(" --- coff optional header 0x%06lx ---\n", optional_header->offset);
  printf("          magic: 0x%04x\n", optional_header->magic);
  printf("        version: 0x%04x\n", optional_header->version);
  printf("      text_size: %d\n", optional_header->text_size);
  printf("      data_size: %d\n", optional_header->data_size);
  printf("   uininit_size: %d\n", optional_header->uninit_size);
  printf("    entry_point: 0x%06x\n", optional_header->entry_point);
  printf("     text_start: 0x%06x\n", optional_header->text_start);
  printf("     data_start: 0x%06x\n", optional_header->data_start);
  printf("\n");
}

void print_coff_section_header(struct coff_section_header_t *section_header)
{
  printf(" --- coff section header 0x%06lx ---\n", section_header->offset);
  printf("                       name: %.8s\n", section_header->name);
  printf("           physical_address: 0x%06x\n", section_header->physical_address);
  printf("            virtual_address: 0x%06x\n", section_header->virtual_address);
  printf("                       size: %d\n", section_header->size);
  printf("   file_offset_section_data: 0x%06x\n", section_header->file_offset_section_data);
  printf("file_offset_relocation_data: 0x%06x\n", section_header->file_offset_relocation_table);
  printf("    file_offset_line_number: 0x%06x\n", section_header->file_offset_line_number);
  printf("          line_number_count: %d\n", section_header->line_number_count);
  printf("                      flags: %08x\n", section_header->flags);
  printf("\n");
}

int print_symbol(FILE *in)
{
  while (1)
  {
    int ch = getc(in);
    if (ch == 0 || ch == EOF) { break; }
    printf("%c", ch);
  }

  return 0;
}

int print_symbol_offsets(FILE *in, uint32_t size)
{
  long marker = ftell(in);
  int symbol_count = read_uint32_be(in);
  uint32_t *offsets = (uint32_t *)malloc(symbol_count * sizeof(uint32_t));
  int n;

  printf(" --- symbol_count: %d ---\n", symbol_count);

  for (n = 0; n < symbol_count; n++)
  {
    offsets[n] = read_uint32_be(in);
  }

  for (n = 0; n < symbol_count; n++)
  {
    printf("   0x%06x  ", offsets[n]);
    print_symbol(in);
    printf("\n");
  }

  printf("\n");

  free(offsets);

  long total = ftell(in) - marker;

  if (total != size)
  {
    printf("Error: Bad archive members size %ld vs %d.\n", total, size);
    return -1;
  }

  return 0;
}

int print_symbol_header(FILE *in)
{
  struct coff_symbol_header_t symbol_header;

  symbol_header.offset = ftell(in);
  symbol_header.version = read_uint16(in);
  symbol_header.machine = read_uint16(in);
  symbol_header.timestamp = read_uint32(in);
  symbol_header.size = read_uint32(in);
  symbol_header.unknown_0 = read_uint16(in);
  symbol_header.unknown_1 = read_uint16(in);

  printf("--- symbol %06lx ---\n", symbol_header.offset);
  printf("        version: 0x%04x\n", symbol_header.version);
  printf("        machine: 0x%04x\n", symbol_header.machine);
  printf("      timestamp: %d\n", symbol_header.timestamp);
  printf("           size: %d\n", symbol_header.size);
  printf("      unknown_0: %d\n", symbol_header.unknown_0);
  printf("      unknown_1: %d\n", symbol_header.unknown_1);
  printf("           name: ");
  print_symbol(in);
  printf("\n");
  printf("            DLL: ");
  print_symbol(in);
  printf("\n\n");

  return 0;
}

int print_public_symbols(FILE *in, uint32_t size)
{
  long marker = ftell(in);
  int offset_count = read_uint32(in);
  int n;

  printf(" --- offset_count: %d ---\n", offset_count);

  for (n = 0; n < offset_count; n++)
  {
    printf("  %02x %06x\n", n, read_uint32(in));
  }

  printf("\n");

  int symbol_count = read_uint32(in);
  uint16_t *offsets = (uint16_t *)malloc(symbol_count * sizeof(uint16_t));

  printf(" --- symbol_count: %d ---\n", symbol_count);

  for (n = 0; n < symbol_count; n++)
  {
    offsets[n] = read_uint16(in);
  }

  for (n = 0; n < symbol_count; n++)
  {
    printf("   0x%04x  ", offsets[n]);
	    print_symbol(in);
	    printf("\n");
	  }

	  printf("\n");

	  long total = ftell(in) - marker;

	  if (total != size)
	  {
	    printf("Error: Bad archive members size %ld vs %d.\n", total, size);
	    return -1;
	  }

	  return 0;
	}

	int print_name(FILE *in, uint32_t size)
	{
	  long marker = ftell(in);

	  printf("   name: ");
	  print_symbol(in);
  printf("\n\n");

  long total = ftell(in) - marker;

  if (total != size)
  {
    printf("Error: Bad archive members size %ld vs %d.\n", total, size);
    return -1;
  }

  return 0;
}

int print_raw(FILE *in, long offset, uint32_t size)
{
  long marker = ftell(in);
  fseek(in, offset, SEEK_SET);
  int n, i;

  uint8_t data[16];
  int total = 0;

  for (n = 0; n < size; n += 16)
  {
    int length = fread(data, 1, 16, in);
    printf("  %06x: ", n);

    total += length;
    if (total > size) { length -= total - size; }

    for (i = 0; i < length; i++)
    {
      printf("%02x ", data[i]);
    }

    for (i = length; i < 16; i++) { printf("   "); }

    for (i = 0; i < length; i++)
    {
      printf("%c", data[i] >= 32 && data[i] < 126 ? data[i] : '.');
    }

    printf("\n");
  }

  printf("\n");

  fseek(in, marker, SEEK_SET);

  return 0;
}

int print_section(FILE *in, uint32_t size)
{
  struct coff_header_t coff_header;
  struct coff_optional_header_t optional_header;
  struct coff_section_header_t section_header;
  long marker = ftell(in);
  int n;

  int result = read_coff_header(in, &coff_header);

  if (result != 0)
  {
    return result;
  }

  print_coff_header(&coff_header);

  if (coff_header.optional_header_size != 0)
  {
    read_coff_optional_header(in, &optional_header);
    print_coff_optional_header(&optional_header);
  }

  for (n = 0; n < coff_header.section_count; n++)
  {
    read_coff_section_header(in, &section_header);
    print_coff_section_header(&section_header);
    print_raw(in, marker + section_header.file_offset_section_data, section_header.size);
  }

#if 0
  long total = ftell(in) - marker;

  if (total != size)
  {
    printf("Error: Bad archive members size %ld vs %d.\n", total, size);
    return -1;
  }
#endif

  fseek(in, marker + size, SEEK_SET);

  return 0;
}

int read_lib(FILE *in)
{
  char signature[8];
  struct file_header_t file_header;
  int size;

  if (fread(signature, 1, 8, in) != 8 ||
      memcmp(signature, "!<arch>\n", 8) != 0)
  {
    printf("Error: Missing !<arch> signature, not a .lib file.\n");
    return -1;
  }

  if (read_file_header(in, &file_header) != 0) { return -1; }
  print_file_header(&file_header);

  size = atoi(file_header.size);
  if (print_symbol_offsets(in, size) != 0) { return -1; }

  while (1)
  {
    if (read_file_header(in, &file_header) != 0) { break; }
    print_file_header(&file_header);

    size = atoi(file_header.size);

    if (memcmp(file_header.name, "/               ", 16) == 0)
    {
      if (print_public_symbols(in, size) != 0) { return -1; }
    }
      else
    if (memcmp(file_header.name, "//              ", 16) == 0)
    {
      if (print_name(in, size) != 0) { return -1; }
    }
      else
    if (memcmp(file_header.name, "/0              ", 16) == 0)
    {
      int result = print_section(in, size);

      if (result == -2)
      {
        print_symbol_header(in);
      }
        else
      if (result != 0)
      {
        return -1;
      }
    }
      else
    {
      printf("Unknown section type.\n");
      fseek(in, size, SEEK_CUR);
    }

    if (memcmp(file_header.end, "`\n", 2) != 0)
    {
      printf("Error: Invalid end marker.\n");
      return -1;
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  FILE *in;

  printf("\nprint_lib (March 24, 2023)\n");
  printf("Copyright 2023 - Michael Kohn  http://www.mikekohn.net/\n\n");

  if (argc != 2)
  {
    printf("Usage: print_lib <filename>\n");
    exit(0);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Could not open input file %s for reading.\n", argv[1]);
    exit(1);
  }

  read_lib(in);

  fclose(in);

  return 0;
}

