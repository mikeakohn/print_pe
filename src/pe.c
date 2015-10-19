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
#include "pe.h"

int rip_binary(FILE *in, char *filename, int address, int size)
{
  FILE *out;
  long marker;
  int t;

  out = fopen(filename,"wb");

  if (out == 0)
  {
    printf("Cannot open file %s for writing.\n\n",filename);
    return -1;
  } 

  marker = ftell(in);
  fseek(in, address, SEEK_SET);

  for (t = 0; t < size; t++)
  {
    putc(getc(in), out);
  }

  fseek(in, marker, SEEK_SET);
  fclose(out);

  return 0;
}

int read_unicode(FILE *in, int addr, char *s, int max_chars)
{
  long marker;
  int ch,t,len;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  t = 0;
  len = read_uint16(in);

  while(t<len)
  {
    getc(in);
    ch = getc(in);
    if (ch == EOF || ch == 0) break;
    s[t++] = ch;

    if (t == max_chars-1) break;
  }

  s[t] = 0;

  fseek(in,marker,SEEK_SET);

  return t;
}

int print_imports(FILE *in, int addr, int size, struct section_header_t *section_header)
{
  struct import_dir_t import_dir;
  char name[1024];
  long marker;
  int total_size;
  int virtual_address,raw_ptr;
  int func_addr;
  int t,ptr;

  virtual_address = section_header->VirtualAddress;
  raw_ptr = section_header->PointerToRawData;

  addr = (addr - virtual_address) + raw_ptr;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

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

int print_exports(FILE *in, int addr, int size, struct section_header_t *section_header, struct funct_t *funct)
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

  printf("   Characteristics: 0x%08x\n",export_dir.Characteristics);
  printf("     TimeDateStamp: %s",ctime((time_t *)&export_dir.TimeDateStamp));
  printf("      MajorVersion: %d\n",export_dir.MajorVersion);
  printf("      MinorVersion: %d\n",export_dir.MinorVersion);
  printf("              Name: ");
  print_string(in, (export_dir.Name - virtual_address) + raw_ptr);
  printf("\n");
  printf("              Base: %d\n",export_dir.Base);
  printf(" NumberOfFunctions: %d\n",export_dir.NumberOfFunctions);
  printf("     NumberOfNames: %d\n",export_dir.NumberOfNames);
  printf("AddressOfFunctions: %d\n",export_dir.AddressOfFunctions);
  printf("    AddressOfNames: %d\n",export_dir.AddressOfNames);
  printf("AddrOfNameOrdinals: %d\n",export_dir.AddressOfNameOrdinals);
  printf("\n");

  printf("     Function Name                   Address     Ordinal\n");

  for (t = 0; t < export_dir.NumberOfNames; t++)
  {
    ptr = get_ptr(in,(export_dir.AddressOfNames-virtual_address)+raw_ptr+(t*4));
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

