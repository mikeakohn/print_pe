/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_H
#define PE_H

#include <stdint.h>

#include "section_header.h"

int rip_binary(FILE *in, char *filename, int address, int size);
int read_unicode(FILE *in, int addr, char *s, int max_chars);
int hex_dump(FILE *in, int address, int size, struct section_header_t *section_header);

#endif

