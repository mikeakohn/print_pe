/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "clsid.h"
#include "fileio.h"

int clsid_read(char *clsid, FILE *in)
{
  int8_t transpose[16] =
  {
     3,  2,  1,  0,
     5,  4,  7,  6,
     8,  9, 10, 11,
    12, 13, 14, 15
  };

  int t;

  for (t = 0; t < 16; t++)
  {
    clsid[(int)transpose[t]] = getc(in);
  }

  return 0;
}

int clsid_print(const uint8_t *clsid)
{
  int t;

  for (t = 0; t < 16; t++)
  {
    printf("%02X", clsid[t]);

    if (t == 3 || t == 5 || t == 7 || t == 9) { printf("-"); }
  }

  return 0;
}

