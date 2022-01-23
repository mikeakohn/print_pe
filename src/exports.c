/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "exports.h"

int exports_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header,
  struct funct_t *funct)
{
  struct export_dir_t export_dir;
  char func_name[1024];
  int virtual_address,raw_ptr;
  int func_addr,name_ord;
  long marker;
  int t,ptr;

  virtual_address = section_header->VirtualAddress;
  raw_ptr = section_header->PointerToRawData;

  addr = (addr - virtual_address) + raw_ptr;
  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  printf("  -- Exported Symbols --\n\n");

  export_dir.Characteristics = read_uint32(in);
  export_dir.TimeDateStamp = read_uint32(in);
  export_dir.MajorVersion = read_uint16(in);
  export_dir.MinorVersion = read_uint16(in);
  export_dir.Name = read_uint32(in);
  export_dir.Base = read_uint32(in);
  export_dir.NumberOfFunctions = read_uint32(in);
  export_dir.NumberOfNames = read_uint32(in);
  export_dir.AddressOfFunctions = read_uint32(in);
  export_dir.AddressOfNames = read_uint32(in);
  export_dir.AddressOfNameOrdinals = read_uint32(in);

  printf("   Characteristics: 0x%08x\n", export_dir.Characteristics);
  printf("     TimeDateStamp: %s", ctime((time_t *)&export_dir.TimeDateStamp));
  printf("      MajorVersion: %d\n", export_dir.MajorVersion);
  printf("      MinorVersion: %d\n", export_dir.MinorVersion);
  printf("              Name: ");
  print_string(in, (export_dir.Name - virtual_address) + raw_ptr);
  printf("\n");
  printf("              Base: %d\n", export_dir.Base);
  printf(" NumberOfFunctions: %d\n", export_dir.NumberOfFunctions);
  printf("     NumberOfNames: %d\n", export_dir.NumberOfNames);
  printf("AddressOfFunctions: 0x%04x\n", export_dir.AddressOfFunctions);
  printf("    AddressOfNames: 0x%04x\n", export_dir.AddressOfNames);
  printf("AddrOfNameOrdinals: 0x%04x\n", export_dir.AddressOfNameOrdinals);
  printf("\n");

  printf("     Function Name                   Address     Ordinal\n");

  for (t = 0; t < export_dir.NumberOfNames; t++)
  {
    ptr = get_ptr(in, (export_dir.AddressOfNames - virtual_address) + raw_ptr + (t * 4));
    get_string(in, func_name, (ptr - virtual_address) + raw_ptr);

    func_addr = get_ptr(in, (export_dir.AddressOfFunctions - virtual_address) + raw_ptr + (t * 4));
    name_ord = get_ushort(in, (export_dir.AddressOfNameOrdinals - virtual_address) + raw_ptr + (t * 2));

    printf("     %-30s  0x%08x  0x%04x\n", func_name, func_addr, name_ord);

    if (funct->funct_name[0] != 0)
    {
      if (strcmp(funct->funct_name, func_name) == 0)
      {
        funct->funct_ptr = func_addr;
      }
    }
  }

  printf("\n");

  fseek(in, marker, SEEK_SET);

  return 0;
}

