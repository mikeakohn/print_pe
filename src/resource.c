/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fileio.h"
#include "resource.h"

#define RES_INDENT_MAX 100
#define RES_INDENT_UNIT 4
#define RES_INDENT_LEVELS (RES_INDENT_MAX/RES_INDENT_UNIT-1)

#define RT_VERSION 0x10

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

static int parse_unicode(FILE *in, char *key, int length)
{
  int ch, w;
  int len = 0;
  int ptr = 0;

  while (1)
  {
    w = getc(in);
    ch = getc(in);
    w |= (ch << 8);
    len += 2;
    if (w == 0 || ch == EOF) { break; }

    printf("%c", w);
    if (ptr < length -1) { key[ptr++] = w; }
  }

  key[ptr] = 0;
  printf("\n");

  return len;
}

static int read_padding(FILE *in, int len)
{
  int count = 0;

  while ((len & 0x3) != 0)
  {
    getc(in);
    count++;
    len++;
  }

  return count;
}

static int read_struct_info(
  FILE *in,
  uint16_t *wLength,
  uint16_t *wValueLength,
  uint16_t *wType,
  char *szKey,
  int szKey_length)
{
  int len = 6;

  *wLength = read_uint16(in);
  *wValueLength = read_uint16(in);
  *wType = read_uint16(in);

  printf("   wLength=%d\n", *wLength);
  printf("   wValueLength=%d\n", *wValueLength);
  printf("   wType=%d %s\n", *wType, *wType == 0 ? "(binary)" : "(text)");
  printf("   szKey=");
  len += parse_unicode(in, szKey, szKey_length);
  len += read_padding(in, len);
  printf("\n");

  return len;
}

static int parse_string(FILE *in)
{
  uint16_t wLength, wValueLength, wType;
  char szKey[128];
  int len, n;

  printf("    -- String --\n");
  len = read_struct_info(in, &wLength, &wValueLength, &wType, szKey, sizeof(szKey));

  printf("   value=");

  for (n = 0; n < wValueLength; n++)
  {
    int w = getc(in);
    int ch = getc(in);
    w |= (ch << 8);

    printf("%c", w);

    len += 2;
  }

  printf("\n\n");

  len += read_padding(in, len);

  return len;
}

static int parse_string_table(FILE *in, int length)
{
  uint16_t wLength, wValueLength, wType;
  char szKey[128];
  int len;

  printf("    -- StringTable --\n");
  len = read_struct_info(in, &wLength, &wValueLength, &wType, szKey, sizeof(szKey));

  while (len < wLength)
  {
    len += parse_string(in);
  }

  return len;
}

static int parse_var(FILE *in, int length)
{
  uint16_t wLength, wValueLength, wType;
  char szKey[128];
  int len;

  printf("    -- Var --\n");
  len = read_struct_info(in, &wLength, &wValueLength, &wType, szKey, sizeof(szKey));

  printf("   value=%d\n\n", read_uint32(in));

  len += 4;

  return len;
}

static int parse_x_file_info(FILE *in)
{
  uint16_t wLength, wValueLength, wType;
  char szKey[128];
  int len;

  printf("   -- StringFileInfo --\n");
  len = read_struct_info(in, &wLength, &wValueLength, &wType, szKey, sizeof(szKey));

  if (strcmp(szKey, "StringFileInfo") == 0)
  {
    len += parse_string_table(in, wLength);
  }
    else
  if (strcmp(szKey, "VarFileInfo") == 0)
  {
    len += parse_var(in, wLength);
  }
    else
  {
    len += wLength - 4;
  }

  return len;
}

int parse_version_info(FILE *in, long offset, int size)
{
  struct fixed_file_info_t fixed_file_info;
  long marker;
  uint16_t wLength, wValueLength, wType;
  char szKey[128];
  int len;

  marker = ftell(in);
  fseek(in, offset, SEEK_SET);

  len = read_struct_info(in, &wLength, &wValueLength, &wType, szKey, sizeof(szKey));

#if 0
fseek(in, offset, SEEK_SET);
//char buffer[2192];
char buffer[wLength];
fread(buffer, 1, sizeof(buffer), in);
fclose(in);

FILE *fp = fopen("vs_versionfinfo.bin", "wb");
fwrite(buffer, 1, sizeof(buffer), fp);
fclose(fp);
exit(0);
#endif

  len += read_padding(in, len);
  len += read_fixed_file_info(in, &fixed_file_info);
  print_fixed_file_info(&fixed_file_info);
  len += read_padding(in, len);

  while (len < wLength)
  {
    len += parse_x_file_info(in);
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

int parse_resource_dir(
  FILE *in,
  struct section_header_t *section_header,
  int offset,
  int level,
  int res_type,
  int dump)
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
    read_resource_dir_entry(
      in,
      section_header->PointerToRawData,
      &resource_dir_entry,
      offset);

    print_resource_dir_entry(&resource_dir_entry, level, t);

    if ((resource_dir_entry.OffsetToData & 0x80000000) == 0)
    {
      // High bit 0: Address of a resource data entry (a leaf).
      read_resource_data(
        in,
        section_header->PointerToRawData,
        &resource_data,
        resource_dir_entry.OffsetToData);

      print_resource_data(&resource_data, level);

      sprintf(filename, "res%d.bin", resource_data.OffsetToData);

      if (dump == 1)
      {
        rip_binary(
          in,
          filename,
          section_header->PointerToRawData + (resource_data.OffsetToData - section_header->VirtualAddress),
          resource_data.Size);
      }

      if (res_type == RT_VERSION)
      {
        parse_version_info(
          in,
          section_header->PointerToRawData + (resource_data.OffsetToData - section_header->VirtualAddress),
          resource_data.Size);
      }

      if (level == 0)
      {
        print_resource_type(resource_dir_entry.Name, level);
      }
    }
      else
    {
      // High bit 1: The lower 31 bits are the address of another
      // resource directory table (the next level down).
      if (level == 0)
      {
        print_resource_type(resource_dir_entry.Name, level);

        parse_resource_dir(
          in,
          section_header,
          resource_dir_entry.OffsetToData & 0x7fffffff,
          level + 1,
          resource_dir_entry.Name,
          dump);
      }
        else
      {
        parse_resource_dir(
          in,
          section_header,
          resource_dir_entry.OffsetToData & 0x7fffffff,
          level + 1,
          res_type,
          dump);
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

int read_resource_dir_entry(
  FILE *in,
  int addr,
  struct resource_dir_entry_t *resource_dir_entry,
  int offset)
{
  long marker;

  marker = ftell(in);
  fseek(in, addr + offset, SEEK_SET);

  resource_dir_entry->Name = read_uint32(in);
  resource_dir_entry->OffsetToData = read_uint32(in);

  if ((resource_dir_entry->Name & 0x80000000) != 0)
  {
    read_unicode(
      in,
      addr + (resource_dir_entry->Name & 0x7fffffff),
      resource_dir_entry->RealName,
      256);
  }
    else
  {
    resource_dir_entry->RealName[0] = 0;
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

int read_resource_data(
  FILE *in,
  int addr,
  struct resource_data_t *resource_data,
  int offset)
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

int read_fixed_file_info(FILE *in, struct fixed_file_info_t *fixed_file_info)
{
  fixed_file_info->Signature = read_uint32(in);
  fixed_file_info->StrucVersion = read_uint32(in);
  fixed_file_info->FileVersionMS = read_uint32(in);
  fixed_file_info->FileVersionLS = read_uint32(in);
  fixed_file_info->ProductVersionMS = read_uint32(in);
  fixed_file_info->ProductVersionLS = read_uint32(in);
  fixed_file_info->FileFlagsMask = read_uint32(in);
  fixed_file_info->FileFlags = read_uint32(in);
  fixed_file_info->FileOS = read_uint32(in);
  fixed_file_info->FileType = read_uint32(in);
  fixed_file_info->FileSubtype = read_uint32(in);
  fixed_file_info->FileDateMS = read_uint32(in);
  fixed_file_info->FileDateLS = read_uint32(in);

  return 13 * sizeof(uint32_t);
}

int print_resource_dir_entry(
  struct resource_dir_entry_t *resource_dir_entry,
  int level,
  int count)
{
  char indent[RES_INDENT_MAX];

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) level = RES_INDENT_LEVELS;
  indent[level * RES_INDENT_UNIT] = 0;

  if ((resource_dir_entry->OffsetToData & 0x80000000) == 0)
  {
    printf("%s-- Resource Dir Entry [%d] --\n", indent, count);
  }
    else
  {
    printf("%s-- Resource Dir Entry (Dir) [%d] --\n", indent, count);
  }

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
  if (level > RES_INDENT_LEVELS) { level = RES_INDENT_LEVELS; }
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
  if (level > RES_INDENT_LEVELS) { level = RES_INDENT_LEVELS; }
  indent[level*RES_INDENT_UNIT] = 0;

  printf("%s>> Resource Type: ", indent);

  if (id < 0x17)
  {
    printf("%s\n", ids[id]);
  }
  else
  {
    printf("???\n");
  }

  printf("\n");

  return 0;
}

int print_resource_data(struct resource_data_t *resource_data, int level)
{
  char indent[RES_INDENT_MAX];

  memset(indent, ' ', RES_INDENT_MAX);
  if (level > RES_INDENT_LEVELS) { level = RES_INDENT_LEVELS; }
  indent[level * RES_INDENT_UNIT] = 0;

  printf("%s-- Resource Data --\n", indent);
  printf("%s|     OffsetToData: %d\n", indent,resource_data->OffsetToData);
  printf("%s|             Size: %d\n", indent,resource_data->Size);
  printf("%s|         CodePage: %d\n", indent,resource_data->CodePage);
  printf("%s|         Reserved: %d\n", indent,resource_data->Reserved);
  printf("\n");

  return 0;
}

int print_fixed_file_info(struct fixed_file_info_t *fixed_file_info)
{
   const char *file_os;
   const char *file_type;

   switch (fixed_file_info->FileOS)
   {
     case 0x00000000: file_os = "(UNKNOWN)"; break;
     case 0x00000001: file_os = "(WIN16)"; break;
     case 0x00000002: file_os = "(PM32)"; break;
     case 0x00000004: file_os = "(WIN32)"; break;
     case 0x00010000: file_os = "(DOS)"; break;
     case 0x00020000: file_os = "(OS216)"; break;
     case 0x00030000: file_os = "(OS232)"; break;
     case 0x00040000: file_os = "(NT)"; break;
     default: file_os = "()"; break;
   }

   switch (fixed_file_info->FileType)
   {
     case 0x00000000: file_type = "(UNKNOWN)"; break;
     case 0x00000001: file_type = "(Application)"; break;
     case 0x00000002: file_type = "(DLL)"; break;
     case 0x00000003: file_type = "(DRV)"; break;
     case 0x00000004: file_type = "(FONT)"; break;
     case 0x00000005: file_type = "(VXD)"; break;
     case 0x00000007: file_type = "(STATIC_LIB)"; break;
     default: file_type = "()"; break;
   }

   printf("        -- VS_FIXEDFILEINFO --\n");
   printf("         Signature: 0x%08x\n", fixed_file_info->Signature);
   printf("      StrucVersion: %d.%d\n",
     fixed_file_info->StrucVersion >> 16,
     fixed_file_info->StrucVersion & 0xffff);
   printf("     FileVersionMS: 0x%x\n", fixed_file_info->FileVersionMS);
   printf("     FileVersionLS: 0x%x\n", fixed_file_info->FileVersionLS);
   printf("  ProductVersionMS: 0x%x\n", fixed_file_info->ProductVersionMS);
   printf("  ProductVersionLS: 0x%x\n", fixed_file_info->ProductVersionLS);
   printf("     FileFlagsMask: 0x%08x\n", fixed_file_info->FileFlagsMask);
   printf("         FileFlags: 0x%08x\n", fixed_file_info->FileFlags);
   printf("            FileOS: %d %s\n", fixed_file_info->FileOS, file_os);
   printf("          FileType: %d %s\n", fixed_file_info->FileType, file_type);
   printf("       FileSubtype: %d\n", fixed_file_info->FileSubtype);
   printf("        FileDateMS: %d\n", fixed_file_info->FileDateMS);
   printf("        FileDateLS: %d\n", fixed_file_info->FileDateLS);
   printf("\n");

   return 0;
}

