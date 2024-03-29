/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_VB_H
#define PE_VB_H

#include <stdint.h>

#include "optional_header.h"
#include "section_header.h"

struct vb_header_t
{
  char szVbMagic[5];
  uint16_t wRuntimeBuild;
  char szLangDll[15];
  char szSecLangDll[15];
  uint16_t wRuntimeRevision;
  uint32_t dwLCID;
  uint32_t dwSecLCID;
  uint32_t lpSubMain;
  uint32_t lpProjectData;
  uint32_t fMdlIntCtls;
  uint32_t fMdlIntCtls2;
  uint32_t dwThreadFlags;
  uint32_t dwThreadCount;
  uint16_t wFormCount;
  uint16_t wExternalCount;
  uint32_t dwThunkCount;
  uint32_t lpGuiTable;
  uint32_t lpExternalTable;
  uint32_t lpComRegisterData;
  uint32_t bSzProjectDescription;
  uint32_t bSzProjectExeName;
  uint32_t bSzProjectHelpFile;
  uint32_t bSzProjectName;
};

int vb_info_print(
  FILE *in,
  struct optional_header_t *optional_header,
  struct section_header_t *section_header);

#endif

