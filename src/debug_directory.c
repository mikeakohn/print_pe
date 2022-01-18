/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "debug_directory.h"
#include "fileio.h"

static int debug_directory_entry_read(
  struct debug_directory_t *debug_directory,
  FILE *in)
{
  debug_directory->characteristics = read_uint32(in);
  debug_directory->time_date_stamp = read_uint32(in);
  debug_directory->major_version = read_uint16(in);
  debug_directory->minor_version = read_uint16(in);
  debug_directory->type = read_uint32(in);
  debug_directory->size_of_data = read_uint32(in);
  debug_directory->address_of_raw_data = read_uint32(in);
  debug_directory->pointer_to_raw_data = read_uint32(in);

  return 0;
}

static int debug_directory_entry_print(
  struct debug_directory_t *debug_directory)
{
  char *debug_types[] =
  {
    "Unknown",
    "COFF",
    "CODEVIEW",
    "FPO",
    "MISC",
    "EXCEPTION",
    "FIXUP",
    "OMAP_TO_SRC",
    "OMAP_FROM_SRC",
    "BORLAND",
    "RESERVED10"
  };

  printf("   Characteristics: %d\n", debug_directory->characteristics);
  printf("     TimeDateStamp: %s", ctime((time_t *)&debug_directory->time_date_stamp));
  printf("      MajorVersion: %d\n", debug_directory->major_version);
  printf("      MinorVersion: %d\n", debug_directory->minor_version);
  printf("              Type: %d (", debug_directory->type);

  if (debug_directory->type < 11)
  {
    printf("%s", debug_types[debug_directory->type]);
  }
    else
  {
    printf("???");
  }

  printf(")\n");
  printf("        SizeOfData: %d\n", debug_directory->size_of_data);
  printf("  AddressOfRawData: 0x%04x\n", debug_directory->address_of_raw_data);
  printf("  PointerToRawData: 0x%04x\n", debug_directory->pointer_to_raw_data);
  printf("\n");

  return 0;
}

static void debug_directory_print_codeview(
  struct debug_directory_t *debug_directory,
  FILE *in)
{
  long marker = ftell(in);
  uint8_t magic[4];
  int n;

  fseek(in, debug_directory->pointer_to_raw_data, SEEK_SET);
  read_chars_bin(in, magic, 4);

  if (memcmp(magic, "RSDS", 4) != 0) { return; }

  printf("      GUID:");
  for (n = 0; n < 16; n++)
  {
    printf(" %02x", getc(in));
  }
  printf("\n");

  uint32_t age = read_uint32(in);
  printf("       age: %d\n", age);
  printf("  filename: ");

  while (1)
  {
    int ch = getc(in);
    if (ch == 0) { break; }
    printf("%c", ch);
  }
  printf("\n\n");

  fseek(in, marker, SEEK_SET);
}

int debug_directory_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header)
{
  struct debug_directory_t debug_directory;
  int n;

  uint32_t virtual_address = section_header->VirtualAddress;
  uint32_t raw_ptr = section_header->PointerToRawData;

  addr = (addr - virtual_address) + raw_ptr;
  long marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  printf("  -- Debug Directory --\n\n");

  for (n = 0; n < size; n += 28)
  {
    printf("  -- Debug Directory Entry %d --\n", n / 28);

    debug_directory_entry_read(&debug_directory, in);
    debug_directory_entry_print(&debug_directory);

    if (debug_directory.type == 2)
    {
      debug_directory_print_codeview(&debug_directory, in);
    }
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

