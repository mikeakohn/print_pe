/*

print_pdb / print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "fileio.h"
#include "pdb.h"

int main(int argc, char *argv[])
{
  FILE *in;
  const char *filename = NULL;
  struct pdb_header_t pdb_header;
  struct pdb_dir_t pdb_dir;
  struct pdb_dbi_t pdb_dbi;
  uint32_t n;

  memset(&pdb_header, 0, sizeof(pdb_header));
  memset(&pdb_dir, 0, sizeof(pdb_dir));

  printf("\nprint_pdb (January 11, 2022)\n");
  printf("Copyright 2005-2022 - Michael Kohn  http://www.mikekohn.net/\n\n");

  for (n = 1; n < argc; n++)
  {
    if (argv[n][0] == '-')
    {
      {
        printf("Error: Unknown option %s\n", argv[n]);
        exit(1);
      }
    }
      else
    {
      filename = argv[n];
    }
  }

  if (filename == NULL)
  {
    printf("Usage: print_pe [options] <input file>\n");
#if 0
    printf("  -start <address>        (starting memory address to dump)\n");
    printf("  -end <address>          (ending memory address to dump)\n");
    printf("  -count <count>          (length of region to dump)\n");
    printf("  -word_size <8/16/32/64> (word size of data to dump)\n");
#endif
    exit(0);
  }

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    printf("Could not open input file %s for reading.\n", argv[1]);
    exit(1);
  }

  if (read_pdb_header(&pdb_header, in) != 0)
  {
    printf("Error: Not recognized as a PDB file.\n");
    fclose(in);
    exit(1);
  }

  print_pdb_header(&pdb_header);

  pdb_dir.stream_sizes = (uint32_t *)malloc(pdb_header.number_of_dir_bytes);

  read_pdb_dir(&pdb_dir, &pdb_header, in);
  print_pdb_dir(&pdb_dir);
  print_pdb_stream_info(&pdb_dir, &pdb_header, in);
  print_pdb_tpi_stream(&pdb_dir, &pdb_header, in, 2);
  print_pdb_tpi_stream(&pdb_dir, &pdb_header, in, 4);
  print_pdb_dbi_stream(&pdb_dir, &pdb_header, &pdb_dbi, in);
  //print_pdb_names(&pdb_dir, &pdb_header, in);
  //print_pdb_global(&pdb_dir, &pdb_header, in);
  //print_pdb_global(&pdb_dir, &pdb_header, in);

  //dump_pdb_stream(&pdb_dir, &pdb_header, in, 1, "index_1.bin");

  free(pdb_dir.stream_sizes);
  free(pdb_dir.heap);
  fclose(in);

  return 0;
}

