/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cil.h"
#include "fileio.h"
#include "pe.h"
#include "pe_debug.h"
#include "pe_vb.h"

int main(int argc, char *argv[])
{
  FILE *in,*out;
  struct dos_header_t dos_header;
  struct image_file_header_t image_file_header;
  struct image_optional_header_t image_optional_header;
  struct section_header_t section_header;
  uint8_t signature[4];
  char filename[64];
  char pe_filename[1024];
  struct funct_t funct;
  int t,len;

  printf("\nprint_pe (December 21, 2014) - The DLL, EXE, OCX Analyzer\n");
  printf("Copyright 2005-2014 - Michael Kohn  http://www.mikekohn.net/\n\n");

  if (argc < 2)
  {
    printf("Usage: print_pe [ options ] <input file>\n");
    printf("       -modify <function name> <new return value>\n\n");
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

  memset(&image_optional_header, 0, sizeof(image_optional_header));

  read_dos_header(in, &dos_header);
  print_dos_header(&dos_header);

  len = dos_header.e_lfanew-ftell(in);
  out = fopen("dos_code.bin","wb");
  if (out != 0)
  { 
    for (t = 0; t < len; t++) putc(getc(in), out);
    fclose(out);
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
      signature[2] != 0 || signature[3] != 0)
  {
    printf("This file is not a Microsoft PE format file\n\n");
    fclose(in);
    exit(0);
  }

  printf("Signature: PE00\n\n");

  read_image_file_header(in, &image_file_header);
  print_image_file_header(&image_file_header);

  if (image_file_header.SizeOfOptionalHeader != 0)
  {
    read_image_optional_header(in, &image_optional_header, image_file_header.SizeOfOptionalHeader);
    print_image_optional_header(&image_optional_header);
  }

  for (t = 0; t < image_file_header.NumberOfSections; t++)
  {
    read_section_header(in, &section_header);
    print_section_header(&section_header, t);

    if (strncmp(section_header.name, ".text", 5) == 0)
    {
      funct.file_offset = section_header.VirtualAddress - section_header.PointerToRawData;

      //struct _clr_header clr_header;
      //read_clr_header(in, &clr_header, 0x200);
      //print_clr_header(&clr_header);
    }

    if ((section_header.Characteristics & 0x20) != 0)
    {
      sprintf(filename, "win_code_%d.bin", t);
      rip_binary(in, filename,section_header.PointerToRawData, section_header.SizeOfRawData);
      //i = print_vb_info(in, &image_optional_header, &section_header);
      print_vb_info(in, &image_optional_header, &section_header);
    }
      else
    if (strcmp(section_header.name, ".rsrc") == 0)
    {
      parse_resource_dir(in, &section_header, 0, 0, 0);
    }

    if (image_optional_header.image_data_dir[3] != 0)
    {
      if (section_header.VirtualAddress <= image_optional_header.image_data_dir[2] &&
          image_optional_header.image_data_dir[2] <= section_header.VirtualAddress+section_header.SizeOfRawData)
      {
        print_imports(in, image_optional_header.image_data_dir[2], image_optional_header.image_data_dir[3], &section_header);
      }
    }

    if (image_optional_header.image_data_dir[1] != 0)
    {
      if (section_header.VirtualAddress <= image_optional_header.image_data_dir[0] &&
          image_optional_header.image_data_dir[0] <= section_header.VirtualAddress+section_header.SizeOfRawData)
      {
        print_exports(in, image_optional_header.image_data_dir[0], image_optional_header.image_data_dir[1], &section_header, &funct);
      }
    }

    if (image_optional_header.image_data_dir[13] != 0)
    {
      if (section_header.VirtualAddress <= image_optional_header.image_data_dir[12] &&
          image_optional_header.image_data_dir[12] <= section_header.VirtualAddress+section_header.SizeOfRawData)
      {
        print_debug_section(in, image_optional_header.image_data_dir[12], image_optional_header.image_data_dir[13], &section_header);
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


