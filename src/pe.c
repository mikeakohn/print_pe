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

char dir_entries[16][21] = {
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
    "" };

int rip_binary(FILE *in, char *filename, int address, int size)
{
  FILE *out;
  int marker;
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
  int marker;
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

int read_dos_header(FILE *in, struct dos_header_t *dos_header)
{
  dos_header->e_magic = read_uint16(in);
  dos_header->e_cblp = read_uint16(in);
  dos_header->e_cp = read_uint16(in);
  dos_header->e_crlc = read_uint16(in);
  dos_header->e_cparhdr = read_uint16(in);
  dos_header->e_minalloc = read_uint16(in);
  dos_header->e_maxalloc = read_uint16(in);
  dos_header->e_ss = read_uint16(in);
  dos_header->e_sp = read_uint16(in);
  dos_header->e_csum = read_uint16(in);
  dos_header->e_ip = read_uint16(in);
  dos_header->e_cs = read_uint16(in);
  dos_header->e_lfarlc = read_uint16(in);
  dos_header->e_ovno = read_uint16(in);
  read_chars_bin(in, dos_header->e_res, 8);
  dos_header->e_oemid = read_uint16(in);
  dos_header->e_oeminfo = read_uint16(in);
  read_chars_bin(in, dos_header->e_res2, 20);
  dos_header->e_lfanew = read_uint32(in);

  return 0;
}

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

/* Hi Mr. Source Code snooper! */

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

int read_section_header(FILE *in, struct section_header_t *section_header)
{
  read_chars(in, section_header->name, 8);
  section_header->Misc.PhysicalAddress = read_uint32(in);
  section_header->VirtualAddress = read_uint32(in);
  section_header->SizeOfRawData = read_uint32(in);
  section_header->PointerToRawData = read_uint32(in);
  section_header->PointerToRelocations = read_uint32(in);
  section_header->PointerToLinenumbers = read_uint32(in);
  section_header->NumberOfRelocations = read_uint16(in);
  section_header->NumberOfLinenumbers = read_uint16(in);
  section_header->Characteristics = read_uint32(in);

  return 0;
}

int print_imports(FILE *in, int addr, int size, struct section_header_t *section_header)
{
  struct import_dir_t import_dir;
  char name[1024];
  int marker;
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
  int marker;
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

int print_dos_header(struct dos_header_t *dos_header)
{
  printf("---------------------------------------------\n");
  printf("MS DOS Header\n");
  printf("---------------------------------------------\n");

  printf("      Magic Number: %c%c\n",dos_header->e_magic&255,dos_header->e_magic>>8);
  printf("Bytes On Last Page: %d\n",dos_header->e_cblp);
  printf("     Pages In File: %d\n",dos_header->e_cp);
  printf("       Relocations: %d\n",dos_header->e_crlc);
  printf("    Size Of Header: %d\n",dos_header->e_cparhdr);
  printf("    Min Extra Para: %d\n",dos_header->e_minalloc);
  printf("    Max Extra Para: %d\n",dos_header->e_maxalloc);
  printf("  Initial SS Value: %d\n",dos_header->e_ss);
  printf("  Initial SP Value: %d\n",dos_header->e_sp);
  printf("          Checksum: %d\n",dos_header->e_csum);
  printf("  Initial IP Value: %d\n",dos_header->e_ip);
  printf("  Initial CS Value: %d\n",dos_header->e_cs);
  printf("  Addr Reloc Table: %d\n",dos_header->e_lfarlc);
  printf("    Overlay Number: %d\n",dos_header->e_ovno);
  /* read_chars_bin(in,dos_header->e_res,4); */
  printf("            OEM ID: %d\n",dos_header->e_oemid);
  printf("          OEM Info: %d\n",dos_header->e_oeminfo);
  /* read_chars_bin(in,dos_header->e_res2,10); */
  printf("Addr Of New Header: %d\n",dos_header->e_lfanew);
  printf("\n");

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
  printf("   Characteristics: %04x", image_file_header->Characteristics);
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

  printf("             Magic: %d\n",image_optional_header->Magic);
  printf("MajorLinkerVersion: %d\n",image_optional_header->MajorLinkerVersion);
  printf("MinorLinkerVersion: %d\n",image_optional_header->MinorLinkerVersion);
  printf("        SizeOfCode: %d\n",image_optional_header->SizeOfCode);
  printf("SizeOfInitilzdData: %d\n",image_optional_header->SizeOfInitializedData);
  printf("SizeOfUnintlzdData: %d\n",image_optional_header->SizeOfUninitializedData);
  printf("  AddrOfEntryPoint: %d\n",image_optional_header->AddressOfEntryPoint);
  printf("        BaseOfCode: %d\n",image_optional_header->BaseOfCode);
  printf("        BaseOfData: %d\n",image_optional_header->BaseOfData);
  printf("         ImageBase: %d\n",image_optional_header->ImageBase);
  printf("  SectionAlignment: %d\n",image_optional_header->SectionAlignment);
  printf("     FileAlignment: %d\n",image_optional_header->FileAlignment);
  printf("    MajorOSVersion: %d\n",image_optional_header->MajorOperatingSystemVersion);
  printf("    MinorOSVersion: %d\n",image_optional_header->MinorOperatingSystemVersion);
  printf(" MajorImageVersion: %d\n",image_optional_header->MajorImageVersion);
  printf(" MinorImageVersion: %d\n",image_optional_header->MinorImageVersion);
  printf(" MajorSubsystemVer: %d\n",image_optional_header->MajorSubsystemVersion);
  printf(" MinorSubsystemVer: %d\n",image_optional_header->MinorSubsystemVersion);
  printf("         Reserved1: %d\n",image_optional_header->Reserved1);
  printf("       SizeOfImage: %d\n",image_optional_header->SizeOfImage);
  printf("     SizeOfHeaders: %d\n",image_optional_header->SizeOfHeaders);
  printf("          CheckSum: %d\n",image_optional_header->CheckSum);
  printf("         Subsystem: %d",image_optional_header->Subsystem);
  if (image_optional_header->Subsystem == 2) printf(" (GUI)");
    else
  if (image_optional_header->Subsystem == 3) printf(" (Console)");
    else
  if (image_optional_header->Subsystem == 3) printf(" (None)");
  printf("\n");
  printf("DllCharacteristics: %d\n",image_optional_header->DllCharacteristics);
  printf("SizeOfStackReserve: %d\n",image_optional_header->SizeOfStackReserve);
  printf(" SizeOfStackCommit: %d\n",image_optional_header->SizeOfStackCommit);
  printf(" SizeOfHeapReserve: %d\n",image_optional_header->SizeOfHeapReserve);
  printf("  SizeOfHeapCommit: %d\n",image_optional_header->SizeOfHeapCommit);
  printf("       LoaderFlags: %d\n",image_optional_header->LoaderFlags);
  printf("  NumOfRvaAndSizes: %d\n",image_optional_header->NumberOfRvaAndSizes);
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

int print_section_header(struct section_header_t *section_header, int count)
{
  printf("---------------------------------------------\n");
  printf("Section Header %d\n",count);
  printf("---------------------------------------------\n");
  printf("      Section Name: %s\n",section_header->name);
  printf("PhyslAddr/VirtSize: %d\n",section_header->Misc.PhysicalAddress);
  printf("    VirtualAddress: %d\n",section_header->VirtualAddress);
  printf("     SizeOfRawData: %d\n",section_header->SizeOfRawData);
  printf("  PointerToRawData: %d\n",section_header->PointerToRawData);
  printf("  PtrToRelocations: %d\n",section_header->PointerToRelocations);
  printf("  PtrToLinenumbers: %d\n",section_header->PointerToLinenumbers);
  printf("  NumOfRelocations: %d\n",section_header->NumberOfRelocations);
  printf("  NumOfLinenumbers: %d\n",section_header->NumberOfLinenumbers);
  printf("   Characteristics: 0x%x\n",section_header->Characteristics);
  printf("\n");

  return 0;
}

int print_clsid(unsigned char *clsid)
{
  int t;

  for (t = 0; t < 16; t++)
  {
    printf("%02X", clsid[t]);
    if (t == 3 || t == 5 || t == 7 || t == 9) printf("-");
  }

  return 0;
}

int read_clsid(FILE *in, char *clsid)
{
  int8_t transpose[16] = { 3,2,1,0, 5,4,7,6, 8,9,10,11, 12,13,14,15 };
  int t;

  for (t = 0; t < 16; t++)
  {
    clsid[(int)transpose[t]] = getc(in);
  }

  return 0;
}

int read_reg_info(FILE *in, struct reg_info_t *reg_info, int offset)
{
  int marker;

  marker = ftell(in);
  fseek(in, offset, SEEK_SET);

  reg_info->bNextObject = read_uint32(in);
  reg_info->bObjectName = read_uint32(in);
  reg_info->bObjectDescription = read_uint32(in);
  reg_info->dwInstancing = read_uint32(in);
  reg_info->dwObjectId = read_uint32(in);
  read_clsid(in,(char *)reg_info->uuidObject);
  reg_info->fIsInterface = read_uint32(in);
  reg_info->bUidObjectIFace = read_uint32(in);
  reg_info->bUidEventsIFace = read_uint32(in);
  reg_info->fHasEvents = read_uint32(in);
  reg_info->dwMiscStatus = read_uint32(in);
  reg_info->fClassType = read_uint32(in);
  reg_info->fObjectType = read_uint16(in);
  reg_info->wToolboxBitmap32 = read_uint16(in);
  reg_info->wDefaultIcon = read_uint16(in);
  reg_info->fIsDesigner = read_uint16(in);
  reg_info->bDesignerData = read_uint32(in);

  fseek(in, marker, SEEK_SET);

  return 0;
}

int print_reg_info(struct reg_info_t *reg_info)
{
  printf("  -- COM Reg Info --\n");
  printf("       bNextObject: %d\n",reg_info->bNextObject);
  printf("       bObjectName: %d (%s)\n",reg_info->bObjectName,reg_info->szObjectName);
  printf("bObjectDescription: %d\n",reg_info->bObjectDescription);
  printf("      dwInstancing: %d\n",reg_info->dwInstancing);
  printf("        dwObjectId: %d\n",reg_info->dwObjectId);
  printf("        uuidObject: ");
  print_clsid(reg_info->uuidObject);
  printf("\n");
  printf("      fIsInterface: %d\n",reg_info->fIsInterface);
  printf("   bUidObjectIFace: %d\n",reg_info->bUidObjectIFace);
  printf("   bUidEventsIFace: %d\n",reg_info->bUidEventsIFace);
  printf("        fHasEvents: %d\n",reg_info->fHasEvents);
  printf("      dwMiscStatus: %d\n",reg_info->dwMiscStatus);
  printf("        fClassType: %d\n",reg_info->fClassType);
  printf("       fObjectType: %d\n",reg_info->fObjectType);
  printf("  wToolboxBitmap32: %d\n",reg_info->wToolboxBitmap32);
  printf("      wDefaultIcon: %d\n",reg_info->wDefaultIcon);
  printf("       fIsDesigner: %d\n",reg_info->fIsDesigner);
  printf("     bDesignerData: %d\n",reg_info->bDesignerData);
  printf("\n");

  return 0;
}

int read_com_reg_data(FILE *in, struct com_reg_data_t *com_reg_data, int offset)
{
  int marker;

  marker = ftell(in);
  fseek(in, offset, SEEK_SET);

  com_reg_data->bRegInfo = read_uint32(in);
  com_reg_data->bSZProjectName = read_uint32(in);
  com_reg_data->bSZHelpDirectory = read_uint32(in);
  com_reg_data->bSZProjectDescription = read_uint32(in);
  read_clsid(in,(char *)com_reg_data->uuidProjectClsId);
  com_reg_data->dwTlbLcid = read_uint32(in);
  com_reg_data->wUnknown = read_uint16(in);
  com_reg_data->wTlbVerMajor = read_uint16(in);
  com_reg_data->wTlbVerMinor = read_uint16(in);

  fseek(in, marker, SEEK_SET);

  return 0;
}

int print_com_reg_data(struct com_reg_data_t *com_reg_data)
{
  printf("  -- COM Reg Data --\n");
  printf("          bRegInfo: %d\n",com_reg_data->bRegInfo);
  printf("    bSZProjectName: %d\n",com_reg_data->bSZProjectName);
  printf("  bSZHelpDirectory: %d\n",com_reg_data->bSZHelpDirectory);
  printf("bSZProjectDescrptn: %d\n",com_reg_data->bSZProjectDescription);
  printf("  uuidProjectClsId: ");
  print_clsid(com_reg_data->uuidProjectClsId);
  printf("\n");
  printf("         dwTlbLcid: %d\n",com_reg_data->dwTlbLcid);
  printf("          wUnknown: %d\n",com_reg_data->wUnknown);
  printf("      wTlbVerMajor: %d\n",com_reg_data->wTlbVerMajor);
  printf("      wTlbVerMinor: %d\n",com_reg_data->wTlbVerMinor);
  printf("\n");

  return 0;
}

int print_vb_info(FILE *in, struct image_optional_header_t *image_optional_header, struct section_header_t *section_header)
{
  struct vb_header_t vb_header;
  struct com_reg_data_t com_reg_data;
  struct reg_info_t reg_info;
  int marker,ptr;
  int t;
  int addr;

  marker = ftell(in);
  fseek(in, (image_optional_header->AddressOfEntryPoint - section_header->VirtualAddress) + section_header->PointerToRawData, SEEK_SET);

  ptr = getc(in);
  /* Compiled VB executables / DLLs have a couple of entry point signitures. */
  /* 0x68, 0x90, 0x5A */
  switch(ptr)
  {
    case 0x5A: /* 0x5A = POP EDX, this means the file might be a VB DLL or OCX. */
	    /* The pointer to the VB5! data stuff is in the first exported function. Usually DllCanUnloadNow()
	     * Once we get to that address it turns into case 0x68 again. */
	    /* Get the address to the export directory */
	    addr = (image_optional_header->image_data_dir[0] - section_header->VirtualAddress) + section_header->PointerToRawData;
	    /* ExportDirectory->AddressOfFunctions = addr+28; */
	    addr = get_ptr(in, get_ptr(in,(addr + 28)));
	    fseek(in, addr + 2, SEEK_SET); /* read past 0x58, 0x68 */
	    ptr = read_uint16(in);
	    break;
    case 0x90: /* 0x90 = NOP, then it's case 0x68 */
	    getc(in); /* read the NOP then fall through to case 0x68 */
    case 0x68: /* 0x68 = PUSH (word) */
	    ptr = read_uint16(in);
	    break;
    default:
	    fseek(in, marker, SEEK_SET);
	    return -2;
	    break;
  }

  if (section_header->VirtualAddress <= ptr &&
      ptr < section_header->VirtualAddress + section_header->SizeOfRawData)
  {
    fseek(in, (ptr - section_header->VirtualAddress) + section_header->PointerToRawData, SEEK_SET);
  }
    else
  {
    fseek(in, marker, SEEK_SET);
    return -3;
  }

  read_chars(in, vb_header.szVbMagic, 4);

  if (strcmp(vb_header.szVbMagic, "VB5!") != 0)
  { 
    fseek(in, marker, SEEK_SET);
    return -1;
  }

  vb_header.wRuntimeBuild = read_uint16(in);
  read_chars(in, vb_header.szLangDll, 14);
  read_chars(in, vb_header.szSecLangDll, 14);
  vb_header.wRuntimeRevision = read_uint16(in);
  vb_header.dwLCID = read_uint32(in);
  vb_header.dwSecLCID = read_uint32(in);
  vb_header.lpSubMain = read_uint32(in);
  vb_header.lpProjectData = read_uint32(in);
  vb_header.fMdlIntCtls = read_uint32(in);
  vb_header.fMdlIntCtls2 = read_uint32(in);
  vb_header.dwThreadFlags = read_uint32(in);
  vb_header.dwThreadCount = read_uint32(in);
  vb_header.wFormCount = read_uint16(in);
  vb_header.wExternalCount = read_uint16(in);
  vb_header.dwThunkCount = read_uint32(in);
  vb_header.lpGuiTable = read_uint32(in);
  vb_header.lpExternalTable = read_uint32(in);
  vb_header.lpComRegisterData = read_uint32(in);
  vb_header.bSzProjectDescription = read_uint32(in);
  vb_header.bSzProjectExeName = read_uint32(in);
  vb_header.bSzProjectHelpFile = read_uint32(in);
  vb_header.bSzProjectName = read_uint32(in);

  printf("  -- VB Header --\n");
  printf("         szVbMagic: %s\n",vb_header.szVbMagic);
  printf("     wRuntimeBuild: %d\n",vb_header.wRuntimeBuild);
  printf("         szLangDll: %s\n",vb_header.szLangDll);
  printf("      szSecLangDll: %s\n",vb_header.szSecLangDll);
  printf("  wRuntimeRevision: %d\n",vb_header.wRuntimeRevision);
  printf("            dwLCID: %d\n",vb_header.dwLCID);
  printf("         dwSecLCID: %d\n",vb_header.dwSecLCID);
  printf("         lpSubMain: %d\n",vb_header.lpSubMain);
  printf("     lpProjectData: %d\n",vb_header.lpProjectData);
  printf("       fMdlIntCtls: 0x%08x\n",vb_header.fMdlIntCtls);
  printf("      fMdlIntCtls2: 0x%08x\n",vb_header.fMdlIntCtls2);
  printf("     dwThreadFlags: %d",vb_header.dwThreadFlags);
  printf(" (");
  if ((vb_header.dwThreadFlags & 1) == 1) printf(" ApartmentModel");
  if ((vb_header.dwThreadFlags & 2) == 2) printf(" RequireLicense");
  if ((vb_header.dwThreadFlags & 4) == 4) printf(" Unattended");
  if ((vb_header.dwThreadFlags & 8) == 8) printf(" SingleThreaded");
  if ((vb_header.dwThreadFlags & 16) == 16) printf(" Retained");
  printf(" )\n");
  printf("     dwThreadCount: %d\n",vb_header.dwThreadCount);
  printf("        wFormCount: %d\n",vb_header.wFormCount);
  printf("    wExternalCount: %d\n",vb_header.wExternalCount);
  printf("      dwThunkCount: %d\n",vb_header.dwThunkCount);
  printf("        lpGuiTable: %d\n",vb_header.lpGuiTable);
  printf("   lpExternalTable: %d\n",vb_header.lpExternalTable);
  printf(" lpComRegisterData: %d\n",vb_header.lpComRegisterData);
  printf("    bSzProjectDesc: %d (",vb_header.bSzProjectDescription);
  print_string(in, ((vb_header.bSzProjectDescription+ptr)-section_header->VirtualAddress)+section_header->PointerToRawData);
  printf(")\n");
  printf(" bSzProjectExeName: %d (",vb_header.bSzProjectExeName);
  print_string(in, ((vb_header.bSzProjectExeName + ptr) - section_header->VirtualAddress) + section_header->PointerToRawData);
  printf(")\n");
  printf("bSzProjectHelpFile: %d (",vb_header.bSzProjectHelpFile);
  print_string(in, ((vb_header.bSzProjectHelpFile + ptr) - section_header->VirtualAddress) + section_header->PointerToRawData);
  printf(")\n");
  printf("    bSzProjectName: %d (",vb_header.bSzProjectName);
  print_string(in, ((vb_header.bSzProjectName + ptr) - section_header->VirtualAddress) + section_header->PointerToRawData);
  printf(")\n");
  printf("\n");

  if (vb_header.lpComRegisterData != 0)
  {
    read_com_reg_data(in, &com_reg_data, vb_header.lpComRegisterData - image_optional_header->ImageBase);
    print_com_reg_data(&com_reg_data);

    if (com_reg_data.bRegInfo != 0)
    {
      t = com_reg_data.bRegInfo;
      while(1)
      {
        read_reg_info(in, &reg_info, vb_header.lpComRegisterData - image_optional_header->ImageBase + t);
        get_string(in, (char *)reg_info.szObjectName, vb_header.lpComRegisterData - image_optional_header->ImageBase + reg_info.bObjectName);
        print_reg_info(&reg_info);
        if (reg_info.bNextObject == 0) break;
        t = reg_info.bNextObject;
      }
    }
  }
  
  fseek(in, marker, SEEK_SET);

  return 0;
}

int read_debug_directory(FILE *in, struct debug_directory_t *debug_directory)
{
  debug_directory->Characteristics = read_uint32(in);
  debug_directory->TimeDateStamp = read_uint32(in);
  debug_directory->MajorVersion = read_uint16(in);
  debug_directory->MinorVersion = read_uint16(in);
  debug_directory->Type = read_uint32(in);
  debug_directory->SizeOfData = read_uint32(in);
  debug_directory->AddressOfRawData = read_uint32(in);
  debug_directory->PointerToRawData = read_uint32(in);

  return 0;
}

int print_debug_directory(struct debug_directory_t *debug_directory)
{
  char *debug_types[] = {
    "Unknown", "COFF", "CODEVIEW", "FPO", "MISC", "EXCEPTION", "FIXUP",
    "OMAP_TO_SRC", "OMAP_FROM_SRC", "BORLAND", "RESERVED10"
  };

  printf("-- Debug Directory --\n");
  printf("   Characteristics: %d\n",debug_directory->Characteristics);
  printf("     TimeDateStamp: %s",ctime((time_t *)&debug_directory->TimeDateStamp));
  printf("      MajorVersion: %d\n",debug_directory->MajorVersion);
  printf("      MinorVersion: %d\n",debug_directory->MinorVersion);
  printf("              Type: %d (",debug_directory->Type);

  if (debug_directory->Type < 11)
  { printf("%s", debug_types[debug_directory->Type]); }
    else
  { printf("???"); }

  printf(")\n");
  printf("        SizeOfData: %d\n",debug_directory->SizeOfData);
  printf("  AddressOfRawData: %d\n",debug_directory->AddressOfRawData);
  printf("  PointerToRawData: %d\n",debug_directory->PointerToRawData);
  printf("\n");

  return 0;
}

int print_debug_section(FILE *in, int addr, int size, struct section_header_t *section_header)
{
  struct debug_directory_t debug_directory;
  int marker,t,p,r;

  marker = ftell(in);
  fseek(in,addr,SEEK_SET);

  t = 0;
  while (t < size)
  {
    read_debug_directory(in, &debug_directory);
    print_debug_directory(&debug_directory);

    if (debug_directory.Type == 2)
    {
      p = ftell(in);
      fseek(in, debug_directory.PointerToRawData, SEEK_SET);
      printf("Unknown Header: ");
      for (r = 0; r < 4; r++)
      {
        printf("%08x ", read_uint32(in));
      }
      printf("\n");
      printf("Debug Filename: ");

      while(1)
      {
        r = getc(in);
        if (r == 0) break;
        printf("%c", r);
      }
      printf("\n\n");
      fseek(in, p, SEEK_SET);
    }
    // rip_binary(in, "debug.bin", debug_directory.PointerToRawData, debug_directory.SizeOfData);
    t = t + 28;
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}


