/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_CLSID_H
#define PE_CLSID_H

#include <stdint.h>

int print_clsid(const uint8_t *clsid);
int read_clsid(FILE *in, char *clsid);

#endif

