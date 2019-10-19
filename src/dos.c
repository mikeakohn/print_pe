/*

print_pe - Copyright 2005-2019 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h"
#include "dos.h"

int read_dos_header(FILE *in, struct dos_header_t *dos_header)
{
  dos_header->e_magic = read_uint16(in);
  dos_header->e_cblp = read_uint16(in);
  dos_header->e_cp = read_uint16(in);
  dos_header->e_crlc = read_uint16(in);
  dos_header->e_cparhdr = read_uint16(in);
  dos_header->e_minalloc = read_uint16(in);
  dos_header->e_maxalloc = read_uint16(in);
  dos_header->e_ss = read_uint16(in);
  dos_header->e_sp = read_uint16(in);
  dos_header->e_csum = read_uint16(in);
  dos_header->e_ip = read_uint16(in);
  dos_header->e_cs = read_uint16(in);
  dos_header->e_lfarlc = read_uint16(in);
  dos_header->e_ovno = read_uint16(in);
  read_chars_bin(in, dos_header->e_res, 8);
  dos_header->e_oemid = read_uint16(in);
  dos_header->e_oeminfo = read_uint16(in);
  read_chars_bin(in, dos_header->e_res2, 20);
  dos_header->e_lfanew = read_uint32(in);

  return 0;
}

int print_dos_header(struct dos_header_t *dos_header)
{
  printf("---------------------------------------------\n");
  printf("MS DOS Header\n");
  printf("---------------------------------------------\n");

  printf("      Magic Number: %c%c\n", dos_header->e_magic & 0xff, dos_header->e_magic >> 8);
  printf("Bytes On Last Page: %d\n", dos_header->e_cblp);
  printf("     Pages In File: %d\n", dos_header->e_cp);
  printf("       Relocations: %d\n", dos_header->e_crlc);
  printf("    Size Of Header: %d\n", dos_header->e_cparhdr);
  printf("    Min Extra Para: %d\n", dos_header->e_minalloc);
  printf("    Max Extra Para: %d\n", dos_header->e_maxalloc);
  printf("  Initial SS Value: %d\n", dos_header->e_ss);
  printf("  Initial SP Value: %d\n", dos_header->e_sp);
  printf("          Checksum: %d\n", dos_header->e_csum);
  printf("  Initial IP Value: %d\n", dos_header->e_ip);
  printf("  Initial CS Value: %d\n", dos_header->e_cs);
  printf("  Addr Reloc Table: %d\n", dos_header->e_lfarlc);
  printf("    Overlay Number: %d\n", dos_header->e_ovno);
  /* read_chars_bin(in,dos_header->e_res,4); */
  printf("            OEM ID: %d\n", dos_header->e_oemid);
  printf("          OEM Info: %d\n", dos_header->e_oeminfo);
  /* read_chars_bin(in,dos_header->e_res2,10); */
  printf("Addr Of New Header: %d\n", dos_header->e_lfanew);
  printf("\n");

  return 0;
}

