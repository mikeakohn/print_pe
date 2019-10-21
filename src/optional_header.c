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
#include <inttypes.h>

#include "fileio.h"
#include "optional_header.h"

char *dir_entries[] =
{
  "Export Symbols",
  "Import Symbols",
  "Resources",
  "Exception",
  "Security",
  "Base Relocation",
  "Debug",
  "Copyright",
  "Global Pointer",
  "Thread Local Storage",
  "Load Configuration",
  "Bound Import",
  "Import Address Table",
  "Delay Import",
  "COM descriptor",
};

int optional_header_read(
  struct optional_header_t *optional_header,
  FILE *in,
  int header_size)
{
  int t;

  long marker = ftell(in);

  optional_header->Magic = read_uint16(in);
  optional_header->MajorLinkerVersion = getc(in);
  optional_header->MinorLinkerVersion = getc(in);
  optional_header->SizeOfCode = read_uint32(in);
  optional_header->SizeOfInitializedData = read_uint32(in);
  optional_header->SizeOfUninitializedData = read_uint32(in);
  optional_header->AddressOfEntryPoint = read_uint32(in);
  optional_header->BaseOfCode = read_uint32(in);

  int bitsize = 32;

  if (optional_header->Magic != 0x20b)
  {
    optional_header->BaseOfData = read_uint32(in);
  }
    else
  {
    bitsize = 64;
  }

  // FIXME: header_size for PE32 is 28, PE32+ is 24.  Is this really
  //        needed anyway?
  if (header_size <= 28) { return 0; }

  if (bitsize == 32)
  {
    optional_header->ImageBase = read_uint32(in);
  }
    else
  {
    optional_header->ImageBase = read_uint64(in);
  }

  optional_header->SectionAlignment = read_uint32(in);
  optional_header->FileAlignment = read_uint32(in);
  optional_header->MajorOperatingSystemVersion = read_uint16(in);
  optional_header->MinorOperatingSystemVersion = read_uint16(in);
  optional_header->MajorImageVersion = read_uint16(in);
  optional_header->MinorImageVersion = read_uint16(in);
  optional_header->MajorSubsystemVersion = read_uint16(in);
  optional_header->MinorSubsystemVersion = read_uint16(in);
  optional_header->Reserved1 = read_uint32(in);
  optional_header->SizeOfImage = read_uint32(in);
  optional_header->SizeOfHeaders = read_uint32(in);
  optional_header->CheckSum = read_uint32(in);
  optional_header->Subsystem = read_uint16(in);
  optional_header->DllCharacteristics = read_uint16(in);

  if (bitsize == 32)
  {
    optional_header->SizeOfStackReserve = read_uint32(in);
    optional_header->SizeOfStackCommit = read_uint32(in);
    optional_header->SizeOfHeapReserve = read_uint32(in);
    optional_header->SizeOfHeapCommit = read_uint32(in);
  }
    else
  {
    optional_header->SizeOfStackReserve = read_uint64(in);
    optional_header->SizeOfStackCommit = read_uint64(in);
    optional_header->SizeOfHeapReserve = read_uint64(in);
    optional_header->SizeOfHeapCommit = read_uint64(in);
  }

  optional_header->LoaderFlags = read_uint32(in);
  optional_header->NumberOfRvaAndSizes = read_uint32(in);

  int used_length = (int)(ftell(in) - marker);

  if (header_size > used_length)
  {
    optional_header->DataDirectoryCount = (header_size - used_length) / 8;

    for (t = 0; t < optional_header->DataDirectoryCount; t++)
    {
      optional_header->directory_entry[t].virtual_address = read_uint32(in);
      optional_header->directory_entry[t].size = read_uint32(in);
    }
  }

  return 0;
}

int optional_header_print(struct optional_header_t *optional_header)
{
  int t;

  printf("---------------------------------------------\n");
  printf("Image Optional Header\n");
  printf("---------------------------------------------\n");

  const char *magic;

  switch(optional_header->Magic)
  {
    case 0x10b: magic = "PE32 32 Bit Exe"; break;
    case 0x20b: magic = "PE32+ 64 Bit Exe"; break;
    case 0x107: magic = "ROM"; break;
    default: magic = "Unknown"; break;
  }

  printf("             Magic: 0x%04x (%s)\n", optional_header->Magic, magic);

  printf("MajorLinkerVersion: %d\n", optional_header->MajorLinkerVersion);
  printf("MinorLinkerVersion: %d\n", optional_header->MinorLinkerVersion);
  printf("        SizeOfCode: %d\n", optional_header->SizeOfCode);
  printf("SizeOfInitilzdData: %d\n", optional_header->SizeOfInitializedData);
  printf("SizeOfUnintlzdData: %d\n", optional_header->SizeOfUninitializedData);
  printf("  AddrOfEntryPoint: %d\n", optional_header->AddressOfEntryPoint);
  printf("        BaseOfCode: %d\n", optional_header->BaseOfCode);
  printf("        BaseOfData: %d\n", optional_header->BaseOfData);
  printf("         ImageBase: 0x%" PRIx64 "\n", optional_header->ImageBase);
  printf("  SectionAlignment: %d\n", optional_header->SectionAlignment);
  printf("     FileAlignment: %d\n", optional_header->FileAlignment);
  printf("    MajorOSVersion: %d\n", optional_header->MajorOperatingSystemVersion);
  printf("    MinorOSVersion: %d\n", optional_header->MinorOperatingSystemVersion);
  printf(" MajorImageVersion: %d\n", optional_header->MajorImageVersion);
  printf(" MinorImageVersion: %d\n", optional_header->MinorImageVersion);
  printf(" MajorSubsystemVer: %d\n", optional_header->MajorSubsystemVersion);
  printf(" MinorSubsystemVer: %d\n", optional_header->MinorSubsystemVersion);
  printf("         Reserved1: %d\n", optional_header->Reserved1);
  printf("       SizeOfImage: %d\n", optional_header->SizeOfImage);
  printf("     SizeOfHeaders: %d\n", optional_header->SizeOfHeaders);
  printf("          CheckSum: %d\n", optional_header->CheckSum);

  const char *subsystem;

  switch(optional_header->Subsystem)
  {
    case 1: subsystem = "Native"; break;
    case 2: subsystem = "GUI"; break;
    case 3: subsystem = "Console"; break;
    case 5: subsystem = "OS2 GUI"; break;
    case 7: subsystem = "Posix Console"; break;
    case 9: subsystem = "Windows CE GUI"; break;
    case 10: subsystem = "EFI App"; break;
    case 11: subsystem = "EFI Boot Service Driver"; break;
    case 12: subsystem = "EFI Runtime Driver"; break;
    case 13: subsystem = "EFI ROM"; break;
    case 14: subsystem = "Xbox"; break;
    case 15: subsystem = "Windows Boot Application"; break;
    default: subsystem = "Unknown"; break;
  }

  printf("         Subsystem: %d (%s)\n", optional_header->Subsystem, subsystem);
  printf("DllCharacteristics: 0x%04x", optional_header->DllCharacteristics);

  if ((optional_header->DllCharacteristics & 1) != 0)
  {
    printf(" (Reserved_1)");
  }

  if ((optional_header->DllCharacteristics & 2) != 0)
  {
    printf(" (Reserved_2)");
  }

  if ((optional_header->DllCharacteristics & 4) != 0)
  {
    printf(" (Reserved_4)");
  }

  if ((optional_header->DllCharacteristics & 8) != 0)
  {
    printf(" (Reserved_8)");
  }

  if ((optional_header->DllCharacteristics & 0x40) != 0)
  {
    printf(" (Dyanamic Base)");
  }

  if ((optional_header->DllCharacteristics & 0x80) != 0)
  {
    printf(" (Force Integrity)");
  }

  if ((optional_header->DllCharacteristics & 0x100) != 0)
  {
    printf(" (NX Compatiblity)");
  }

  if ((optional_header->DllCharacteristics & 0x200) != 0)
  {
    printf(" (No Isolation)");
  }

  if ((optional_header->DllCharacteristics & 0x400) != 0)
  {
    printf(" (No Structure Exception Handling)");
  }

  if ((optional_header->DllCharacteristics & 0x800) != 0)
  {
    printf(" (No Bind)");
  }

  if ((optional_header->DllCharacteristics & 0x2000) != 0)
  {
    printf(" (WDM Driver)");
  }

  if ((optional_header->DllCharacteristics & 0x8000) != 0)
  {
    printf(" (Terminal Server Aware)");
  }

  printf("\n");

  printf("SizeOfStackReserve: %" PRId64 "\n", optional_header->SizeOfStackReserve);
  printf(" SizeOfStackCommit: %" PRId64 "\n", optional_header->SizeOfStackCommit);
  printf(" SizeOfHeapReserve: %" PRId64 "\n", optional_header->SizeOfHeapReserve);
  printf("  SizeOfHeapCommit: %" PRId64 "\n", optional_header->SizeOfHeapCommit);
  printf("       LoaderFlags: %d\n", optional_header->LoaderFlags);
  printf("  NumOfRvaAndSizes: %d\n", optional_header->NumberOfRvaAndSizes);
  printf("\n");

  if (optional_header->DataDirectoryCount != 0)
  {
    printf("   Directory Description             VirtualAddr  Size\n");

    for (t = 0; t < optional_header->DataDirectoryCount; t++)
    {
      const char *desc;

      if (t < sizeof(dir_entries) / sizeof(char *))
      {
        desc = dir_entries[t];
      }
        else
      {
        desc = "";
      }

      printf("%2d %-33s 0x%08x   0x%08x (%d)\n", t, desc,
        optional_header->directory_entry[t].virtual_address,
        optional_header->directory_entry[t].size,
        optional_header->directory_entry[t].size);
    }

    if (t != 0) { printf("\n"); }
  }

  return 0;
}

