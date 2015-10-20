/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _TABLE_CIL_INSTR_H
#define _TABLE_CIL_INSTR_H

#include <stdint.h>

struct _table_cil_instr
{
  char *name;
  uint8_t size;
};

extern struct _table_cil_instr table_cil_instr[];
extern struct _table_cil_instr table_cil_instr_fe[];

#endif

