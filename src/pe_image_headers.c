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

#include "fileio.h"
#include "pe_image_headers.h"

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

int read_image_optional_header(FILE *in, struct image_optional_header_t *image_optional_header, int header_size)
{
  int t;

  image_optional_header->Magic = read_uint16(in);
  image_optional_header->MajorLinkerVersion = getc(in);
  image_optional_header->MinorLinkerVersion = getc(in);
  image_optional_header->SizeOfCode = read_uint32(in);
  image_optional_header->SizeOfInitializedData = read_uint32(in);
  image_optional_header->SizeOfUninitializedData = read_uint32(in);
  image_optional_header->AddressOfEntryPoint = read_uint32(in);
  image_optional_header->BaseOfCode = read_uint32(in);
  image_optional_header->BaseOfData = read_uint32(in);

  if (header_size <= 28) { return 0; }

  image_optional_header->ImageBase = read_uint32(in);
  image_optional_header->SectionAlignment = read_uint32(in);
  image_optional_header->FileAlignment = read_uint32(in);
  image_optional_header->MajorOperatingSystemVersion = read_uint16(in);
  image_optional_header->MinorOperatingSystemVersion = read_uint16(in);
  image_optional_header->MajorImageVersion = read_uint16(in);
  image_optional_header->MinorImageVersion = read_uint16(in);
  image_optional_header->MajorSubsystemVersion = read_uint16(in);
  image_optional_header->MinorSubsystemVersion = read_uint16(in);
  image_optional_header->Reserved1 = read_uint32(in);
  image_optional_header->SizeOfImage = read_uint32(in);
  image_optional_header->SizeOfHeaders = read_uint32(in);
  image_optional_header->CheckSum = read_uint32(in);
  image_optional_header->Subsystem = read_uint16(in);
  image_optional_header->DllCharacteristics = read_uint16(in);
  image_optional_header->SizeOfStackReserve = read_uint32(in);
  image_optional_header->SizeOfStackCommit = read_uint32(in);
  image_optional_header->SizeOfHeapReserve = read_uint32(in);
  image_optional_header->SizeOfHeapCommit = read_uint32(in);
  image_optional_header->LoaderFlags = read_uint32(in);
  image_optional_header->NumberOfRvaAndSizes = read_uint32(in);

  if (header_size > 96)
  {
    image_optional_header->DataDirectoryCount = (header_size - 96) / 8;
    for (t = 0; t < image_optional_header->DataDirectoryCount; t++)
    {
      image_optional_header->directory_entry[t].virtual_address = read_uint32(in);
      image_optional_header->directory_entry[t].size = read_uint32(in);
    }
  }

  return 0;
}

int print_image_optional_header(struct image_optional_header_t *image_optional_header)
{
  int t;

  printf("---------------------------------------------\n");
  printf("Image Optional Header\n");
  printf("---------------------------------------------\n");

  const char *magic;

  switch(image_optional_header->Magic)
  {
    case 0x10b: magic = "32 Bit Exe"; break;
    case 0x20b: magic = "64 Bit Exe"; break;
    case 0x107: magic = "ROM"; break;
    default: magic = "Unknown"; break;
  }

  printf("             Magic: 0x%04x (%s)\n", image_optional_header->Magic, magic);

  printf("MajorLinkerVersion: %d\n", image_optional_header->MajorLinkerVersion);
  printf("MinorLinkerVersion: %d\n", image_optional_header->MinorLinkerVersion);
  printf("        SizeOfCode: %d\n", image_optional_header->SizeOfCode);
  printf("SizeOfInitilzdData: %d\n", image_optional_header->SizeOfInitializedData);
  printf("SizeOfUnintlzdData: %d\n", image_optional_header->SizeOfUninitializedData);
  printf("  AddrOfEntryPoint: %d\n", image_optional_header->AddressOfEntryPoint);
  printf("        BaseOfCode: %d\n", image_optional_header->BaseOfCode);
  printf("        BaseOfData: %d\n", image_optional_header->BaseOfData);
  printf("         ImageBase: %d\n", image_optional_header->ImageBase);
  printf("  SectionAlignment: %d\n", image_optional_header->SectionAlignment);
  printf("     FileAlignment: %d\n", image_optional_header->FileAlignment);
  printf("    MajorOSVersion: %d\n", image_optional_header->MajorOperatingSystemVersion);
  printf("    MinorOSVersion: %d\n", image_optional_header->MinorOperatingSystemVersion);
  printf(" MajorImageVersion: %d\n", image_optional_header->MajorImageVersion);
  printf(" MinorImageVersion: %d\n", image_optional_header->MinorImageVersion);
  printf(" MajorSubsystemVer: %d\n", image_optional_header->MajorSubsystemVersion);
  printf(" MinorSubsystemVer: %d\n", image_optional_header->MinorSubsystemVersion);
  printf("         Reserved1: %d\n", image_optional_header->Reserved1);
  printf("       SizeOfImage: %d\n", image_optional_header->SizeOfImage);
  printf("     SizeOfHeaders: %d\n", image_optional_header->SizeOfHeaders);
  printf("          CheckSum: %d\n", image_optional_header->CheckSum);

  const char *subsystem;

  switch(image_optional_header->Subsystem)
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

  printf("         Subsystem: %d (%s)\n", image_optional_header->Subsystem, subsystem);
  printf("DllCharacteristics: 0x%04x", image_optional_header->DllCharacteristics);

  if ((image_optional_header->DllCharacteristics & 1) != 0)
  {
    printf(" (Reserved_1)");
  }

  if ((image_optional_header->DllCharacteristics & 2) != 0)
  {
    printf(" (Reserved_2)");
  }

  if ((image_optional_header->DllCharacteristics & 4) != 0)
  {
    printf(" (Reserved_4)");
  }

  if ((image_optional_header->DllCharacteristics & 8) != 0)
  {
    printf(" (Reserved_8)");
  }

  if ((image_optional_header->DllCharacteristics & 0x40) != 0)
  {
    printf(" (Dyanamic Base)");
  }

  if ((image_optional_header->DllCharacteristics & 0x80) != 0)
  {
    printf(" (Force Integrity)");
  }

  if ((image_optional_header->DllCharacteristics & 0x100) != 0)
  {
    printf(" (NX Compatiblity)");
  }

  if ((image_optional_header->DllCharacteristics & 0x200) != 0)
  {
    printf(" (No Isolation)");
  }

  if ((image_optional_header->DllCharacteristics & 0x400) != 0)
  {
    printf(" (No Structure Exception Handling)");
  }

  if ((image_optional_header->DllCharacteristics & 0x800) != 0)
  {
    printf(" (No Bind)");
  }

  if ((image_optional_header->DllCharacteristics & 0x2000) != 0)
  {
    printf(" (WDM Driver)");
  }

  if ((image_optional_header->DllCharacteristics & 0x8000) != 0)
  {
    printf(" (Terminal Server Aware)");
  }

  printf("\n");

  printf("SizeOfStackReserve: %d\n", image_optional_header->SizeOfStackReserve);
  printf(" SizeOfStackCommit: %d\n", image_optional_header->SizeOfStackCommit);
  printf(" SizeOfHeapReserve: %d\n", image_optional_header->SizeOfHeapReserve);
  printf("  SizeOfHeapCommit: %d\n", image_optional_header->SizeOfHeapCommit);
  printf("       LoaderFlags: %d\n", image_optional_header->LoaderFlags);
  printf("  NumOfRvaAndSizes: %d\n", image_optional_header->NumberOfRvaAndSizes);
  printf("\n");

  if (image_optional_header->DataDirectoryCount != 0)
  {
    printf("   Directory Description             VirtualAddr  Size\n");
    for (t = 0; t < image_optional_header->DataDirectoryCount; t++)
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
        image_optional_header->directory_entry[t].virtual_address,
        image_optional_header->directory_entry[t].size,
        image_optional_header->directory_entry[t].size);
    }

    if (t != 0) { printf("\n"); }
  }

  return 0;
}

