/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cil.h"
#include "debug_directory.h"
#include "dos.h"
#include "exports.h"
#include "file_header.h"
#include "fileio.h"
#include "imports.h"
#include "pe.h"
#include "resource.h"
#include "vb.h"

int main(int argc, char *argv[])
{
  FILE *in,*out;
  struct dos_header_t dos_header;
  struct file_header_t file_header;
  struct optional_header_t optional_header;
  struct section_header_t section_header;
  uint8_t signature[4];
  char filename[64];
  char pe_filename[1024];
  struct funct_t funct;
  int dump = 0;
  int address_size = 4;
  int t, len;

  printf("\nprint_pe (January 15, 2023) - The DLL, EXE, OCX Analyzer\n");
  printf("Copyright 2005-2022 - Michael Kohn  http://www.mikekohn.net/\n\n");

  if (argc < 2)
  {
    printf(
       "Usage: print_pe [ options ] <input file>\n"
       "       -modify <function> <new return value>  (modify function)\n"
       "       -dump                                  (dump binary sections)\n"
       "\n");
    exit(0);
  }

  filename[0] = 0;
  funct.funct_name[0] = 0;
  funct.funct_ptr = 0;
  funct.file_offset = 0;

  for (t = 1; t < argc; t++)
  {
    if (strcasecmp(argv[t], "-modify") == 0)
    {
      strcpy(funct.funct_name, argv[++t]);
      funct.funct_retvalue = atoi(argv[++t]);
    }
      else
    if (strcasecmp(argv[t], "-dump") == 0)
    {
      dump = 1;
    }
      else
    {
      strcpy(pe_filename, argv[t]);
    }
  }

  in = fopen(pe_filename, "rb");

  if (in == 0)
  {
    printf("Cannot open file %s for reading.\n\n", argv[1]);
    exit(0);
  }

  memset(&optional_header, 0, sizeof(optional_header));

  read_dos_header(in, &dos_header);
  print_dos_header(&dos_header);

  len = dos_header.e_lfanew - ftell(in);

  if (dump == 1)
  {
    out = fopen("dos_code.bin", "wb");

    if (out != NULL)
    {
      for (t = 0; t < len; t++) { putc(getc(in), out); }
      fclose(out);
    }
  }

  if (dos_header.e_lfanew == 0)
  {
    printf("This is a DOS program.\n");
    fclose(in);
    exit(0);
  }

  fseek(in, dos_header.e_lfanew, SEEK_SET);

  read_chars_bin(in, signature, 4);

  if (signature[0] != 'P' || signature[1] != 'E' ||
      signature[2] != 0   || signature[3] != 0)
  {
    printf("This file is not a Microsoft PE format file\n\n");
    fclose(in);
    exit(0);
  }

  printf("Signature: PE00\n\n");

  file_header_read(&file_header, in);
  file_header_print(&file_header);

  if (file_header.SizeOfOptionalHeader != 0)
  {
    const int length = file_header.SizeOfOptionalHeader;

    optional_header_read(&optional_header, in, length);
    optional_header_print(&optional_header);
  }

  if (optional_header.Magic == 0x20b) { address_size = 8; }

  for (t = 0; t < file_header.NumberOfSections; t++)
  {
    section_header_read(&section_header, in);
    section_header_print(&section_header, t);

    if (strncmp(section_header.name, ".text", 5) == 0)
    {
      funct.file_offset = section_header.VirtualAddress - section_header.PointerToRawData;

      //struct _clr_header clr_header;
      //read_clr_header(in, &clr_header, 0x200);
      //print_clr_header(&clr_header);
    }

    if ((section_header.Characteristics & 0x20) != 0)
    {
      if (dump == 1)
      {
        sprintf(filename, "win_code_%d.bin", t);

        rip_binary(
          in,
          filename,
          section_header.PointerToRawData,
          section_header.SizeOfRawData);
      }

      vb_info_print(in, &optional_header, &section_header);
    }
      else
    if (strcmp(section_header.name, ".rsrc") == 0)
    {
      parse_resource_dir(in, &section_header, 0, 0, 0, dump);
    }

    const uint32_t virtual_start = section_header.VirtualAddress;
    const uint32_t virtual_end =
      section_header.VirtualAddress +
      section_header.SizeOfRawData;;

    // Print imports
    if (optional_header.directory_entry[1].size != 0)
    {
      if (optional_header.directory_entry[1].virtual_address >= virtual_start &&
          optional_header.directory_entry[1].virtual_address < virtual_end)
      {
        imports_print(
          in,
          optional_header.directory_entry[1].virtual_address,
          optional_header.directory_entry[1].size,
          address_size,
          &section_header);
      }
    }

    // Print exports
    if (optional_header.directory_entry[0].size != 0)
    {
      if (optional_header.directory_entry[0].virtual_address >= virtual_start &&
          optional_header.directory_entry[0].virtual_address < virtual_end)
      {
        exports_print(
          in,
          optional_header.directory_entry[0].virtual_address,
          optional_header.directory_entry[0].size,
          &section_header,
          &funct);
      }
    }

    // Debug section
    if (optional_header.directory_entry[6].size!= 0)
    {
      if (optional_header.directory_entry[6].virtual_address >= virtual_start &&
          optional_header.directory_entry[6].virtual_address < virtual_end)
      {
#if 0
        debug_section_print(
          in,
          optional_header.directory_entry[6].virtual_address,
          optional_header.directory_entry[6].size,
          &section_header);
#endif
        debug_directory_print(
          in,
          optional_header.directory_entry[6].virtual_address,
          optional_header.directory_entry[6].size,
          &section_header);
      }
    }

    // COM Desc section
    if (optional_header.directory_entry[14].size != 0)
    {
      if (optional_header.directory_entry[14].virtual_address >= virtual_start &&
          optional_header.directory_entry[14].virtual_address < virtual_end)
      {

        int is_dot_net = imports_find(
          in,
          optional_header.directory_entry[1].virtual_address,
          optional_header.directory_entry[1].size,
          address_size,
          &section_header,
          "_CorExeMain");

        if (is_dot_net == 1)
        {
          printf("---------------------------------------------\n");
          printf("COM Descriptor\n");
          printf("---------------------------------------------\n");

          hex_dump(
            in,
            optional_header.directory_entry[14].virtual_address,
            optional_header.directory_entry[14].size,
            &section_header);

          struct _clr_header clr_header;

          read_clr_header(
            in,
            &clr_header,
            optional_header.directory_entry[14].virtual_address,
            optional_header.directory_entry[14].size,
            &section_header);

          print_clr_header(&clr_header);
        }
      }
    }
  }

  fclose(in);

  if (funct.funct_name[0] != 0)
  {
    if (funct.funct_ptr == 0)
    {
      printf("Function %s not found in file.\n", funct.funct_name);
    }
      else
    if (funct.file_offset == 0)
    {
      printf("Function %s is found in file, but couldn't find the .text section.\nThis is a bug in print_pe probably.\n", funct.funct_name);
    }
      else
    {
      funct.funct_ptr = funct.funct_ptr - funct.file_offset;
      printf("Function %s found at %ld in file.\n\n", funct.funct_name, funct.funct_ptr);

      printf("This is experimental and will modify your file.\nDo you really want to change this function to { return %d; } (y/n) ", funct.funct_retvalue);

      t = getchar();

      if (t == 'y')
      {
        FILE *out = fopen(pe_filename, "rb+");
        fseek(out, funct.funct_ptr, SEEK_SET);
        putc(0xb8, out);
        putc((funct.funct_retvalue & 255), out);
        putc(((funct.funct_retvalue >> 8) & 255), out);
        putc(((funct.funct_retvalue >> 16) & 255), out);
        putc(((funct.funct_retvalue >> 24) & 255), out);
        putc(0xc3, out);
        fclose(out);
        printf("Modified!\n");
      }
        else
      {
        printf("Cancelled!\n");
      }
    }
  }

  return 0;
}

