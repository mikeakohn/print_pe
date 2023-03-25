/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>

uint64_t read_uint64(FILE *in);
int read_uint32(FILE *in);
int read_uint16(FILE *in);
int read_uint8(FILE *in);
int read_chars(FILE *in, char *s, int count);
int read_chars_bin(FILE *in, uint8_t *s, int count);

int read_uint32_be(FILE *in);
int read_uint16_be(FILE *in);

int print_string(FILE *in, int addr);
int get_string(FILE *in, char *s, int addr);
int get_ptr(FILE *in, int addr);
int get_ushort(FILE *in, int addr);

#endif

