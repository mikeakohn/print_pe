/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef CLSID_H
#define CLSID_H

#include <stdint.h>

int clsid_read(char *clsid, FILE *in);
int clsid_print(const uint8_t *clsid);

#endif

