/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "pe.h"

int rip_binary(FILE *in, char *filename, int address, int size)
{
  FILE *out;
  long marker;
  int t;

  out = fopen(filename,"wb");

  if (out == 0)
  {
    printf("Cannot open file %s for writing.\n\n",filename);
    return -1;
  } 

  marker = ftell(in);
  fseek(in, address, SEEK_SET);

  for (t = 0; t < size; t++)
  {
    putc(getc(in), out);
  }

  fseek(in, marker, SEEK_SET);
  fclose(out);

  return 0;
}

int read_unicode(FILE *in, int addr, char *s, int max_chars)
{
  long marker;
  int ch,t,len;

  marker = ftell(in);
  fseek(in, addr, SEEK_SET);

  t = 0;
  len = read_uint16(in);

  while(t<len)
  {
    getc(in);
    ch = getc(in);
    if (ch == EOF || ch == 0) break;
    s[t++] = ch;

    if (t == max_chars-1) break;
  }

  s[t] = 0;

  fseek(in,marker,SEEK_SET);

  return t;
}

