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
  uint8_t show_dir = 0;
  uint8_t show_tpi = 0;
  uint8_t show_ipi = 0;
  uint8_t show_dbi = 0;
  uint8_t dump_index = 0;
  char *dump_filename = NULL;

  memset(&pdb_header, 0, sizeof(pdb_header));
  memset(&pdb_dir, 0, sizeof(pdb_dir));

  printf("\nprint_pdb (January 15, 2022)\n");
  printf("Copyright 2005-2022 - Michael Kohn  http://www.mikekohn.net/\n\n");

  for (n = 1; n < argc; n++)
  {
    if (argv[n][0] == '-')
    {
      if (strcmp(argv[n], "-all") == 0)
      {
        show_dir = 1;
        show_tpi = 1;
        show_ipi = 1;
      }
        else
      if (strcmp(argv[n], "-dir") == 0)
      {
        show_dir = 1;
      }
        else
      if (strcmp(argv[n], "-tpi") == 0)
      {
        show_tpi = 1;
      }
        else
      if (strcmp(argv[n], "-ipi") == 0)
      {
        show_ipi = 1;
      }
        else
      if (strcmp(argv[n], "-dump") == 0)
      {
        if (n + 2 >= argc)
        {
          printf("Error: -dump requires a filename and index\n");
          exit(1);
        }

        dump_filename = argv[++n];
        dump_index = atoi(argv[++n]);
      }
        else
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
    printf("  -all                     (Show all streams)\n");
    printf("  -dir                     (Show directory stream)\n");
    printf("  -tpi                     (Show TPI stream)\n");
    printf("  -ipi                     (Show IPI stream)\n");
    printf("  -dump <filename> <index> (Dump stream at index to filename)\n");
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

  if (show_dir != 0)
  {
    print_pdb_dir(&pdb_dir);
  }

  print_pdb_stream_info(&pdb_dir, &pdb_header, in);
  read_pdb_names(&pdb_dir, &pdb_header, in);

  if (show_tpi != 0)
  {
    print_pdb_tpi_stream(&pdb_dir, &pdb_header, in, 2);
  }

  if (show_ipi != 0)
  {
    print_pdb_tpi_stream(&pdb_dir, &pdb_header, in, 4);
  }

  if (show_dbi != 0)
  {
    print_pdb_dbi_stream(&pdb_dir, &pdb_header, &pdb_dbi, in);
  }

  //print_pdb_names(&pdb_dir, &pdb_header, in);
  //print_pdb_global(&pdb_dir, &pdb_header, in);
  //print_pdb_global(&pdb_dir, &pdb_header, in);

  if (dump_filename != NULL)
  {
    dump_pdb_stream(&pdb_dir, &pdb_header, in, dump_index, dump_filename);
  }

  free(pdb_dir.stream_sizes);
  free(pdb_dir.heap);
  free(pdb_dir.names);
  fclose(in);

  return 0;
}

