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
  ""
};


int read_image_file_header(FILE *in, struct image_file_header_t *image_file_header)
{
  image_file_header->Machine = read_uint16(in);
  image_file_header->NumberOfSections = read_uint16(in);
  image_file_header->TimeDateStamp = read_uint32(in);
  image_file_header->PointerToSymbolTable = read_uint32(in);
  image_file_header->NumberOfSymbols = read_uint32(in);
  image_file_header->SizeOfOptionalHeader = read_uint16(in);
  image_file_header->Characteristics = read_uint16(in);

  return 0;
}

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

  if (header_size<=28) return 0;

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

  if (header_size>96)
  {
    image_optional_header->DataDirectoryCount = (header_size - 96) / 8;
    for (t = 0; t < image_optional_header->DataDirectoryCount * 2; t++)
    {
      image_optional_header->image_data_dir[t] = read_uint32(in);
    }
  }

  return 0;
}

int print_image_file_header(struct image_file_header_t *image_file_header)
{
  printf("---------------------------------------------\n");
  printf("Image File Header\n");
  printf("---------------------------------------------\n");

  printf("           Machine: ");
  if (image_file_header->Machine == 0x14c) printf("i386");
    else
  if (image_file_header->Machine == 0x14d) printf("i486");
    else
  if (image_file_header->Machine == 0x14e) printf("i586");
    else
  if (image_file_header->Machine == 0x160) printf("MIPS R3000 Big Endian");
    else
  if (image_file_header->Machine == 0x162) printf("MIPS R3000 Little Endian");
    else
  if (image_file_header->Machine == 0x166) printf("MIPS R4000 Little Endian");
    else
  if (image_file_header->Machine == 0x168) printf("MIPS R10000 Little Endian");
    else
  if (image_file_header->Machine == 0x184) printf("Alpha");
    else
  if (image_file_header->Machine == 0x1f0) printf("PPC Little Endian");
    else
  if (image_file_header->Machine == 0x8664) printf("AMD64");
    else
  { printf("Unknown"); }

  printf("\n");
  printf("  NumberOfSections: %d\n", image_file_header->NumberOfSections);
  printf("     TimeDateStamp: %s", ctime((time_t *)&image_file_header->TimeDateStamp));
  printf("PointerToSymbolTbl: %d\n", image_file_header->PointerToSymbolTable);
  printf("   NumberOfSymbols: %d\n", image_file_header->NumberOfSymbols);
  printf(" SizeOfOptionalHdr: %d\n", image_file_header->SizeOfOptionalHeader);
  printf("   Characteristics: 0x%04x", image_file_header->Characteristics);
  if ((image_file_header->Characteristics & 0x0001) != 0)
  {
    printf(" (Relocations Stripped)");
  }

  if ((image_file_header->Characteristics & 0x0002) != 0)
  {
    printf(" (Executable Image)");
  }

  if ((image_file_header->Characteristics & 0x0004) != 0)
  {
    printf(" (Line Numbers Stripped)");
  }

  if ((image_file_header->Characteristics & 0x0008) != 0)
  {
    printf(" (File Local Symbols Stripped)");
  }

  if ((image_file_header->Characteristics & 0x0010) != 0)
  {
    printf(" (Agressively Trim Working Set)");
  }

  if ((image_file_header->Characteristics & 0x0020) != 0)
  {
    printf(" (Large Address Aware)");
  }

  if ((image_file_header->Characteristics & 0x0080) != 0)
  {
    printf(" (File Bytes Reversed)");
  }

  if ((image_file_header->Characteristics & 0x0100) != 0)
  {
    printf(" (32 Bit Machine)");
  }

  if ((image_file_header->Characteristics & 0x0400) != 0)
  {
    printf(" (File Removable, Run From Swap)");
  }

  if ((image_file_header->Characteristics & 0x0800) != 0)
  {
    printf(" (File Networked, Run From Swap)");
  }

  if ((image_file_header->Characteristics & 0x1000) != 0)
  {
    printf(" (File System Image)");
  }

  if ((image_file_header->Characteristics & 0x2000) != 0)
  {
    printf(" (Dynamic Link Library)");
  }

  if ((image_file_header->Characteristics & 0x4000) != 0)
  {
    printf(" (Uniprocessor Only)");
  }

  if ((image_file_header->Characteristics & 0x8000) != 0)
  {
    printf(" (File Bytes Reversed Hi)");
  }

  printf("\n\n");

  return 0;
}

int print_image_optional_header(struct image_optional_header_t *image_optional_header)
{
  int t;

  printf("---------------------------------------------\n");
  printf("Image Optional Header\n");
  printf("---------------------------------------------\n");

  printf("             Magic: 0x%04x", image_optional_header->Magic);

  if (image_optional_header->Magic == 0x10b)
  {
    printf(" (32 Bit Exe)");
  }
    else
  if (image_optional_header->Magic == 0x20b)
  {
    printf(" (64 Bit Exe)");
  }
    else
  if (image_optional_header->Magic == 0x10b)
  {
    printf(" (ROM)");
  }
  printf("\n");

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
  printf("         Subsystem: %d ", image_optional_header->Subsystem);

  const char *subsystem = "(Unknown)";

  if (image_optional_header->Subsystem == 1) { subsystem = "Native"; }
    else
  if (image_optional_header->Subsystem == 2) { subsystem = "GUI"; }
    else
  if (image_optional_header->Subsystem == 3) { subsystem = "Console"; }
    else
  if (image_optional_header->Subsystem == 5) { subsystem = "OS2 GUI"; }
    else
  if (image_optional_header->Subsystem == 7) { subsystem = "Posix Console"; }
    else
  if (image_optional_header->Subsystem == 9) { subsystem = "Windows CE GUI"; }
    else
  if (image_optional_header->Subsystem == 10) { subsystem = "EFI App"; }
    else
  if (image_optional_header->Subsystem == 11) { subsystem = "EFI Boot Service Driver"; }
    else
  if (image_optional_header->Subsystem == 12) { subsystem = "EFI Runtime Driver"; }
    else
  if (image_optional_header->Subsystem == 13) { subsystem = "EFI ROM"; }
    else
  if (image_optional_header->Subsystem == 14) { subsystem = "Xbox"; }
    else
  if (image_optional_header->Subsystem == 15) { subsystem = "Windows Boot Application"; }

  printf("(%s)\n", subsystem);

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
    for (t=0; t<image_optional_header->DataDirectoryCount*2; t=t+2)
    {
      printf("%2d %-33s 0x%08x   0x%08x (%d)\n",t >> 1, dir_entries[t >> 1],
                                image_optional_header->image_data_dir[t],
                                image_optional_header->image_data_dir[t + 1],
                                image_optional_header->image_data_dir[t + 1]);
    }
    if (t != 0) printf("\n");
  }

  return 0;
}

