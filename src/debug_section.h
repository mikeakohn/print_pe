/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef DEBUG_SECTION_H
#define DEBUG_SECTION_H

#include <stdint.h>

#include "section_header.h"

int debug_section_print(FILE *in, int addr, int size, struct section_header_t *section_header);

#endif

