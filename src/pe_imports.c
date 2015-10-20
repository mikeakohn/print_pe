/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "pe_imports.h"

int print_imports(FILE *in, int address, int size, struct section_header_t *section_header)
{
  struct import_dir_t import_dir;
  char name[1024];
  long marker;
  int total_size;
  uint32_t virtual_address,raw_ptr;
  int func_addr;
  int t,ptr;

  virtual_address = section_header->VirtualAddress;
  raw_ptr = section_header->PointerToRawData;

  address = (address - virtual_address) + raw_ptr;

  marker = ftell(in);
  fseek(in, address, SEEK_SET);

  printf("  -- Imported Symbols --\n");

  total_size = 0;

  while(total_size < size)
  {
    import_dir.FunctionNameList = read_uint32(in);
    import_dir.TimeDateStamp = read_uint32(in);
    import_dir.ForwardChain = read_uint32(in);
    import_dir.ModuleName = read_uint32(in);
    import_dir.FunctionAddressList = read_uint32(in);

    if (import_dir.FunctionNameList == 0) break;

    printf("  FunctionNameList: %d\n",import_dir.FunctionNameList);
    printf("     TimeDateStamp: %s",ctime((time_t *)&import_dir.TimeDateStamp));
    printf("      ForwardChain: %d\n",import_dir.ForwardChain);
    get_string(in,name,import_dir.ModuleName);
    printf("        ModuleName: %s\n",name);
    printf("FunctionAddresList: %d\n",import_dir.FunctionAddressList);
    printf("\n");

    printf("     Function Name                   Address\n");

    t=0;
    while(1)
    {
      func_addr = get_ptr(in, (import_dir.FunctionAddressList - virtual_address) + raw_ptr + t);
      if (func_addr == 0) break;
      ptr = get_ptr(in, (import_dir.FunctionNameList - virtual_address) + raw_ptr + t);
      get_string(in, name,(ptr - virtual_address) + raw_ptr + 2);
      printf("     %-30s  0x%08x\n", name, func_addr);

      t = t + 4;
    }

    printf("\n");
    total_size = total_size + sizeof(import_dir);
  }

  fseek(in, marker, SEEK_SET);
  return 0;
}

