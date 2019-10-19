/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug_directory.h"
#include "debug_section.h"
#include "fileio.h"

int debug_section_print(
  FILE *in,
  int addr,
  int size,
  struct section_header_t *section_header)
{
  struct debug_directory_t debug_directory;
  int marker, t, p, r;

  marker = ftell(in);
  fseek(in,addr,SEEK_SET);

  t = 0;

  while (t < size)
  {
    debug_directory_read(&debug_directory, in);
    debug_directory_print(&debug_directory);

    if (debug_directory.Type == 2)
    {
      p = ftell(in);
      fseek(in, debug_directory.PointerToRawData, SEEK_SET);

      printf("Unknown Header: ");

      for (r = 0; r < 4; r++)
      {
        printf("%08x ", read_uint32(in));
      }

      printf("\n");
      printf("Debug Filename: ");

      while(1)
      {
        r = getc(in);
        if (r == 0) break;
        printf("%c", r);
      }
      printf("\n\n");
      fseek(in, p, SEEK_SET);
    }
    // rip_binary(in, "debug.bin", debug_directory.PointerToRawData, debug_directory.SizeOfData);
    t = t + 28;
  }

  fseek(in, marker, SEEK_SET);

  return 0;
}

