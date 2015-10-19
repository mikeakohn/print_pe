/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fileio.h"
#include "pe.h"

#define RES_INDENT_MAX 100
#define RES_INDENT_UNIT 4
#define RES_INDENT_LEVELS (RES_INDENT_MAX/RES_INDENT_UNIT-1)

/*

Resource Types:
1: Cursor
2: Bitmap
3: Icon
4: Menu
5: Dialog
6: String
7: Fontdir
8: Font
9: Accelerator
10: RCData
11: MessageTable
16: Version
17: dlginclude
19: plugplay
20: vxd
21: anicursor
22: aniicon
23: html
*/

int parse_string_file_info(FILE *in)
{
  int wLength,wValueLength,wType;
  int len;
  int ch,t;

  /* wValueLength is WRONG if it's a string and VB6 created this file..
     good job on quality control Microsoft.
  */

  wLength = read_uint16(in);
  wValueLength = read_uint16(in);
  wType = read_uint16(in);
  len = 6;

/*
printf("wLength=%d\n", wLength);
printf("wValueLength=%d\n", wValueLength);
printf("wType=%d\n", wType);
*/

  if (wType != 1)
  {
    fseek(in, wLength - len, SEEK_CUR);
    return wLength;
  }

  while(len < wLength)
  {
    ch = getc(in);
    if (ch == EOF) return 100000;
    getc(in);

    len = len + 2;

    if (ch == 0)
    {
      if (wValueLength != 0)
      {
        while((len % 4) != 0) { getc(in); len++; }
        printf(": ");

        for (t = 0; t < wValueLength; t++)
        {
          ch = getc(in);
          getc(in);

          len = len + 2;
          if (ch == 0) break;
          printf("%c", ch);
        }
      }

      printf("\n");
      while((len % 4) != 0) { getc(in); len++; }

      while (len < wLength)
      {
        len = len + parse_string_file_info(in);
      }

      break;
    }

    printf("%c", ch);
  }

  return len;
}

int parse_version_info(FILE *in, long offset, int size)
{
  long marker;
  int wLength;
  int len;

  marker = ftell(in);
  fseek(in, offset, SEEK_SET);

  wLength = read_uint16(in);
  fseek(in, offset + 0x5c, SEEK_SET);

  len = 0x5c;
  while(len < wLength)
  {
    len = len + parse_string_file_info(in);
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

int parse_resource_dir(FILE *in, struct section_header_t *section_header, int offset, int level, int res_type)
{
  struct resource_dir_t resource_dir;
  struct resource_dir_entry_t resource_dir_entry;
  struct resource_data_t resource_data;
  int count,t;
  char filename[256];

  read_resource_dir(in, section_header->PointerToRawData + offset, &resource_dir);
  print_resource_dir(&resource_dir, level);

  offset = offset + 16;

  count = resource_dir.NumberOfNamedEntries + resource_dir.NumberOfIdEntries;
  for (t = 0; t < count; t++)
  {
    read_resource_dir_entry(in, section_header->PointerToRawData, &resource_dir_entry, offset);
    print_resource_dir_entry(&resource_dir_entry, level);

    if ((resource_dir_entry.OffsetToData & 0x80000000) == 0)
    {
      read_resource_data(in, section_header->PointerToRawData, &resource_data, resource_dir_entry.OffsetToData);
      print_resource_data(&resource_data, level);

      sprintf(filename, "res%d.bin", resource_data.OffsetToData);
      rip_binary(in, filename,section_header->PointerToRawData + (resource_data.OffsetToData - section_header->VirtualAddress), resource_data.Size);

      if (res_type == 0x10)
      {
        parse_version_info(in, section_header->PointerToRawData + (resource_data.OffsetToData - section_header->VirtualAddress), resource_data.Size);
      }

      if (level == 0)
      { print_resource_type(resource_dir_entry.Name, level); }
    }
      else
    {
      if (level == 0)
      {
        print_resource_type(resource_dir_entry.Name,level);
        parse_resource_dir(in, section_header, (resource_dir_entry.OffsetToData & 0x7fffffff), level + 1, resource_dir_entry.Name);
      }
        else
      {
        parse_resource_dir(in, section_header, (resource_dir_entry.OffsetToData & 0x7fffffff), level + 1,res_type);
      }
    }

    offset = offset + 8;
  }

  return 0;
}

int read_resource_dir(FILE *in, int addr, struct resource_dir_t *resource_dir)
{
  int marker;

  marker = ftell(in);
  fseek(in,addr,SEEK_SET);

  resource_dir->Characteristics = read_uint32(in);
  resource_dir->TimeDateStamp = read_uint32(in);
  resource_dir->MajorVersion = read_uint16(in);
  resource_dir->MinorVersion = read_uint16(in);
  resource_dir->NumberOfNamedEntries = read_uint16(in);
  resource_dir->NumberOfIdEntries = read_uint16(in);

  fseek(in, marker, SEEK_SET);
  return 0;
}

int save_resource(FILE *in, char *filename, int addr)
{
  int marker;
  unsigned OffsetToData,Size;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  OffsetToData = read_uint32(in);
  Size = read_uint32(in);

  printf("OffsetToData=%d\n", OffsetToData);
  printf("Size=0x%08x\n", Size);

  fseek(in, marker, SEEK_SET);
  return 0;
}

int read_resource_dir_entry(FILE *in, int addr, struct resource_dir_entry_t *resource_dir_entry, int offset)
{
int marker;

  marker = ftell(in);
  fseek(in, addr + offset, SEEK_SET);

  resource_dir_entry->Name = read_uint32(in);
  resource_dir_entry->OffsetToData = read_uint32(in);

  if ((resource_dir_entry->Name & 0x80000000) != 0)
  { read_unicode(in, addr + (resource_dir_entry->Name & 0x7fffffff), resource_dir_entry->RealName, 256); }
    else
  { resource_dir_entry->RealName[0] = 0; }

  fseek(in, marker, SEEK_SET);

  return 0;
}

int read_resource_data(FILE *in, int addr, struct resource_data_t *resource_data, int offset)
{
  long marker;

  marker = ftell(in);
  fseek(in, addr + offset, SEEK_SET);

  resource_data->OffsetToData = read_uint32(in);
  resource_data->Size = read_uint32(in);
  resource_data->CodePage = read_uint32(in);
  resource_data->Reserved = read_uint32(in);

  fseek(in, marker, SEEK_SET);

  return 0;
}

int print_resource_dir_entry(struct resource_dir_entry_t *resource_dir_entry, int level)
{
  char indent[RES_INDENT_MAX];

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) level = RES_INDENT_LEVELS;
  indent[level * RES_INDENT_UNIT] = 0;

  if ((resource_dir_entry->OffsetToData & 0x80000000) == 0)
  { printf("%s-- Resource Dir Entry --\n", indent); }
    else
  { printf("%s-- Resource Dir Entry (Dir) --\n", indent); }

  printf("%s|             Name: 0x%08x", indent, resource_dir_entry->Name);
  if ((resource_dir_entry->Name&0x80000000) != 0)
  {
    printf("%s|(%s)", indent, resource_dir_entry->RealName);
  }
  printf("\n");
  printf("%s|     OffsetToData: 0x%08x\n", indent, resource_dir_entry->OffsetToData);
  printf("\n");

  return 0;
}

int print_resource_dir(struct resource_dir_t *resource_dir, int level)
{
  char indent[RES_INDENT_MAX];

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) level = RES_INDENT_LEVELS;
  indent[level * RES_INDENT_UNIT] = 0;

  printf("%s|_______ Resource Directory ___\n",indent);

  printf("%s|  Characteristics: 0x%x\n",indent,resource_dir->Characteristics);
  printf("%s|    TimeDateStamp: %s",indent,ctime((time_t *)&resource_dir->TimeDateStamp));
  printf("%s|     MajorVersion: %d\n",indent,resource_dir->MajorVersion);
  printf("%s|     MinorVersion: %d\n",indent,resource_dir->MinorVersion);
  printf("%s|NumOfNamedEntries: %d\n",indent,resource_dir->NumberOfNamedEntries);
  printf("%s|   NumOfIdEntries: %d\n",indent,resource_dir->NumberOfIdEntries);
  printf("\n");

  return 0;
}

int print_resource_type(int id, int level)
{
  char indent[RES_INDENT_MAX];
  char *ids[] = {
    "", "Cursor", "Bitmap", "Icon", "Menu", "Dialog", "String",
    "Font Dir", "Font", "Accelerator", "RCData", "MessageTable",
    "", "", "", "", "Version Info", "DlgInclude", "", "PlugPlay",
    "VXD", "AniCursor", "AniIcon", "HTML", "Unknown"
  };

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) level = RES_INDENT_LEVELS;
  indent[level*RES_INDENT_UNIT] = 0;

  printf("%s>> Resource Type: ", indent);

  if (id < 0x17) { printf("%s\n", ids[id]); }
  else { printf("???\n"); }

  printf("\n");

  return 0;
}

int print_resource_data(struct resource_data_t *resource_data, int level)
{
  char indent[RES_INDENT_MAX];

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) level = RES_INDENT_LEVELS;
  indent[level * RES_INDENT_UNIT] = 0;

  printf("%s-- Resource Data --\n", indent);
  printf("%s|     OffsetToData: %d\n", indent,resource_data->OffsetToData);
  printf("%s|             Size: %d\n", indent,resource_data->Size);
  printf("%s|         CodePage: %d\n", indent,resource_data->CodePage);
  printf("%s|         Reserved: %d\n", indent,resource_data->Reserved);
  printf("\n");

  return 0;
}


