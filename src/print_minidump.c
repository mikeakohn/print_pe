/*

print_minidump / print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "fileio.h"
#include "minidump.h"
#include "minidump_stream_type.h"

int main(int argc, char *argv[])
{
  FILE *in;
  const char *filename = NULL;
  struct minidump_header_t minidump_header;
  struct minidump_dir_t minidump_dir;
  struct memory_print_t memory_print = { 0 };
  uint32_t n, i;
  uint32_t cpu_arch = 0;

  memory_print.word_size = 1;

  printf("\nprint_minidump (January 11, 2022) - MiniDump Crash Analyzer\n");
  printf("Copyright 2005-2022 - Michael Kohn  http://www.mikekohn.net/\n\n");

  for (n = 1; n < argc; n++)
  {
    if (argv[n][0] == '-')
    {
      if (strcmp(argv[n], "-start") == 0)
      {
        memory_print.start = strtoll(argv[++n], NULL, 0);
      }
        else
      if (strcmp(argv[n], "-end") == 0)
      {
        memory_print.end = strtoll(argv[++n], NULL, 0);
      }
        else
      if (strcmp(argv[n], "-count") == 0)
      {
        memory_print.count = strtoll(argv[++n], NULL, 0);
      }
        else
      if (strcmp(argv[n], "-word_size") == 0)
      {
        memory_print.word_size = atoi(argv[++n]);

        switch (memory_print.word_size)
        {
          case 8:  memory_print.word_size = 1; break;
          case 16: memory_print.word_size = 2; break;
          case 32: memory_print.word_size = 4; break;
          case 64: memory_print.word_size = 8; break;
          default:
            printf("Error: Unknown word size %d\n", memory_print.word_size);
            exit(1);
        }
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
    printf("  -start <address>        (starting memory address to dump)\n");
    printf("  -end <address>          (ending memory address to dump)\n");
    printf("  -count <count>          (length of region to dump)\n");
    printf("  -word_size <8/16/32/64> (word size of data to dump)\n");
    exit(0);
  }

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    printf("Could not open input file %s for reading.\n", argv[1]);
    exit(1);
  }

  read_minidump_header(&minidump_header, in);
  print_minidump_header(&minidump_header);

  // Need to find cpu_arch in order to parse ThreadContext.
  fseek(in, minidump_header.ofs_streams, SEEK_SET);
  for (i = 0; i < minidump_header.num_streams; i++)
  {
    read_minidump_dir(&minidump_dir, in);

    if (minidump_dir.stream_type == STREAM_SYSTEM_INFO)
    {
      long marker = ftell(in);
      fseek(in, minidump_dir.ofs_data, SEEK_SET);

      cpu_arch = read_uint16(in);

      fseek(in, marker, SEEK_SET);
    }
  }

  fseek(in, minidump_header.ofs_streams, SEEK_SET);

  for (i = 0; i < minidump_header.num_streams; i++)
  {
    read_minidump_dir(&minidump_dir, in);
    print_minidump_dir(&minidump_dir, i);

    long marker = ftell(in);
    fseek(in, minidump_dir.ofs_data, SEEK_SET);

    switch (minidump_dir.stream_type)
    {
      case STREAM_THREAD_LIST:
        print_minidump_thread_list(in, cpu_arch);
        break;
      case STREAM_MODULE_LIST:
        print_minidump_module_list(in);
        break;
      case STREAM_MEMORY_LIST:
        print_minidump_memory_list(in);
        break;
      case STREAM_EXCEPTION:
        print_minidump_exception(in);
        break;
      case STREAM_SYSTEM_INFO:
        print_minidump_system_info(in);
        break;
      case STREAM_MEMORY64_LIST:
        print_minidump_memory64_list(in, &memory_print);
        break;
      case STREAM_HANDLE_DATA:
        print_minidump_handle_data(in);
        break;
      case STREAM_MISC_INFO:
        print_minidump_misc_info(in);
        break;
      case STREAM_THREAD_INFO_LIST:
        print_minidump_thread_info_list(in);
        break;
      default:
        break;
    }

    fseek(in, marker, SEEK_SET);
  }

  printf("\n");

  if (memory_print.start != 0)
  {
    if (memory_print.file_offset == 0)
    {
      printf("Error: Memory region 0x%" PRIx64 " doesn't exist.\n",
        memory_print.start);
    }
      else
    {
      printf("Found 0x%" PRIx64 " in range 0x%" PRIx64 "-0x%" PRIx64 "\n",
        memory_print.start,
        memory_print.region_start,
        memory_print.region_end);

      if (memory_print.end == 0)
      {
        memory_print.end = memory_print.start + memory_print.word_size;
      }

      uint64_t length = memory_print.end - memory_print.start;
      uint64_t address = memory_print.start;
      uint64_t i;
      int modulo = 1;

      if (memory_print.count != 0)
      {
        length = memory_print.count;
      }

      switch (memory_print.word_size)
      {
        case 1: modulo = 16; break;
        case 2: modulo = 8;  break;
        case 4: modulo = 4;  break;
        case 8: modulo = 1;  break;
      }

      fseek(in, memory_print.file_offset, SEEK_SET);

      for (i = 0; i < length; i += memory_print.word_size)
      {
        if ((i % modulo) == 0)
        {
          printf("\n%08" PRIx64 ":", address);
        }

        switch (memory_print.word_size)
        {
          case 1: printf(" %02x",  (uint8_t)read_uint8(in)); break;
          case 2: printf(" %04x",  read_uint16(in)); break;
          case 4: printf(" %08x",  read_uint32(in)); break;
          case 8: printf(" %016" PRIx64 , read_uint64(in)); break;
        }

        address += memory_print.word_size;
      }

      printf("\n\n");
    }
  }

  fclose(in);

  return 0;
}

