/*

print_minidump / print_pe - Copyright 2005-2020 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio.h"
#include "minidump.h"

int main(int argc, char *argv[])
{
  FILE *in;
  struct minidump_header_t minidump_header;
  struct minidump_dir_t minidump_dir;
  uint32_t i;

  printf("\nprint_minidump (February 8, 2020) - MiniDump Crash Analyzer\n");
  printf("Copyright 2005-2020 - Michael Kohn  http://www.mikekohn.net/\n\n");

  if (argc < 2)
  {
    printf("Usage: print_pe <input file>\n");
    exit(0);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Could not open input file %s for reading.\n", argv[1]);
    exit(1);
  }

  read_minidump_header(&minidump_header, in);
  print_minidump_header(&minidump_header);

  fseek(in, minidump_header.ofs_streams, SEEK_SET);

  for (i = 0; i < minidump_header.num_streams; i++)
  {
    read_minidump_dir(&minidump_dir, in);
    print_minidump_dir(&minidump_dir, i);
  }

  fclose(in);

  return 0;
}

