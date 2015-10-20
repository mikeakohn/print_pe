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
#include "pe_com.h"
#include "pe_vb.h"

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
    case 0x5a: /* 0x5A = POP EDX, this means the file might be a VB DLL or OCX. */
	    /* The pointer to the VB5! data stuff is in the first exported function. Usually DllCanUnloadNow()
	     * Once we get to that address it turns into case 0x68 again. */
	    /* Get the address to the export directory */
	    addr = (image_optional_header->directory_entry[0].virtual_address - section_header->VirtualAddress) + section_header->PointerToRawData;
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
  printf("         szVbMagic: %s\n", vb_header.szVbMagic);
  printf("     wRuntimeBuild: %d\n", vb_header.wRuntimeBuild);
  printf("         szLangDll: %s\n", vb_header.szLangDll);
  printf("      szSecLangDll: %s\n", vb_header.szSecLangDll);
  printf("  wRuntimeRevision: %d\n", vb_header.wRuntimeRevision);
  printf("            dwLCID: %d\n", vb_header.dwLCID);
  printf("         dwSecLCID: %d\n", vb_header.dwSecLCID);
  printf("         lpSubMain: %d\n", vb_header.lpSubMain);
  printf("     lpProjectData: %d\n", vb_header.lpProjectData);
  printf("       fMdlIntCtls: 0x%08x\n", vb_header.fMdlIntCtls);
  printf("      fMdlIntCtls2: 0x%08x\n", vb_header.fMdlIntCtls2);
  printf("     dwThreadFlags: %d", vb_header.dwThreadFlags);
  printf(" (");
  if ((vb_header.dwThreadFlags & 1) == 1) printf(" ApartmentModel");
  if ((vb_header.dwThreadFlags & 2) == 2) printf(" RequireLicense");
  if ((vb_header.dwThreadFlags & 4) == 4) printf(" Unattended");
  if ((vb_header.dwThreadFlags & 8) == 8) printf(" SingleThreaded");
  if ((vb_header.dwThreadFlags & 16) == 16) printf(" Retained");
  printf(" )\n");
  printf("     dwThreadCount: %d\n", vb_header.dwThreadCount);
  printf("        wFormCount: %d\n", vb_header.wFormCount);
  printf("    wExternalCount: %d\n", vb_header.wExternalCount);
  printf("      dwThunkCount: %d\n", vb_header.dwThunkCount);
  printf("        lpGuiTable: %d\n", vb_header.lpGuiTable);
  printf("   lpExternalTable: %d\n", vb_header.lpExternalTable);
  printf(" lpComRegisterData: %d\n", vb_header.lpComRegisterData);
  printf("    bSzProjectDesc: %d (", vb_header.bSzProjectDescription);
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

