/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t read_uint64(FILE *in)
{
  uint64_t c;

  c = (uint64_t)getc(in);
  c |= (uint64_t)getc(in) << 8;
  c |= (uint64_t)getc(in) << 16;
  c |= (uint64_t)getc(in) << 24;
  c |= (uint64_t)getc(in) << 32;
  c |= (uint64_t)getc(in) << 40;
  c |= (uint64_t)getc(in) << 48;
  c |= (uint64_t)getc(in) << 56;

  return c;
}

int read_uint32(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;
  c |= getc(in) << 16;
  c |= getc(in) << 24;

  return c;
}

int read_uint16(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;

  return c;
}

int read_chars(FILE *in, char *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    s[t] = getc(in);
  }

  s[t] = 0;

  return 0;
}

int read_chars_bin(FILE *in, uint8_t *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    s[t] = getc(in);
  }

  return 0;
}

int print_string(FILE *in, int addr)
{
  int marker;
  int ch;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  while(1)
  {
    ch = getc(in);
    if (ch == EOF || ch == 0) { break; }

    printf("%c",ch);
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

int get_string(FILE *in, char *s, int addr)
{
  int marker;
  int ch;
  int t;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  t = 0;

  while(1)
  {
    ch = getc(in);
    if (ch == EOF || ch == 0) { break; }

    s[t++] = ch;
  }

  s[t] = 0;

  fseek(in, marker, SEEK_SET);

  return 0;
}

int get_ptr(FILE *in, int addr)
{
  int marker;
  int i;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  i = read_uint32(in);

  fseek(in, marker, SEEK_SET);
  return i;
}

int get_ushort(FILE *in, int addr)
{
  int marker;
  int i;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  i = read_uint16(in);

  fseek(in, marker, SEEK_SET);
  return i;
}



