/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_COM_H
#define PE_COM_H

#include <stdint.h>

struct com_reg_data_t
{
  uint32_t bRegInfo;
  uint32_t bSZProjectName;
  uint32_t bSZHelpDirectory;
  uint32_t bSZProjectDescription;
  uint8_t uuidProjectClsId[16];
  uint32_t dwTlbLcid;
  uint16_t wUnknown;
  uint16_t wTlbVerMajor;
  uint16_t wTlbVerMinor;
};

struct reg_info_t
{
  uint32_t bNextObject;
  uint32_t bObjectName;
  uint32_t bObjectDescription;
  uint32_t dwInstancing;
  uint32_t dwObjectId;
  uint8_t uuidObject[16];
  uint32_t fIsInterface;
  uint32_t bUidObjectIFace;
  uint32_t bUidEventsIFace;
  uint32_t fHasEvents;
  uint32_t dwMiscStatus;
  uint32_t fClassType;
  uint16_t fObjectType;
  uint16_t wToolboxBitmap32;
  uint16_t wDefaultIcon;
  uint16_t fIsDesigner;
  uint32_t bDesignerData;
  uint8_t szObjectName[1024];
};

int reg_info_read(struct reg_info_t *reg_info, FILE *in, int offset);
int reg_info_print(struct reg_info_t *reg_info);
int com_reg_data_read(struct com_reg_data_t *com_reg_data, FILE *in, int offset);
int com_reg_data_print(struct com_reg_data_t *com_reg_data);

#endif

