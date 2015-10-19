/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>

int read_uint32(FILE *in);
int read_uint16(FILE *in);
int read_chars(FILE *in, char *s, int count);
int read_chars_bin(FILE *in, uint8_t *s, int count);

int print_string(FILE *in, int addr);
int get_string(FILE *in, char *s, int addr);
int get_ptr(FILE *in, int addr);
int get_ushort(FILE *in, int addr);

#endif

