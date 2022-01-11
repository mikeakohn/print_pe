/*

print_minidump / print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "fileio.h"
#include "minidump.h"
#include "minidump_stream_type.h"

int read_minidump_header(struct minidump_header_t *minidump_header, FILE *in)
{
  read_chars(in, minidump_header->magic1, 4);
  read_chars_bin(in, minidump_header->magic2, 2);
  minidump_header->version = read_uint16(in);
  minidump_header->num_streams = read_uint32(in);
  minidump_header->ofs_streams = read_uint32(in);
  minidump_header->checksum = read_uint32(in);
  minidump_header->timestamp = read_uint32(in);
  minidump_header->flags = read_uint64(in);

  return 0;
}

int read_minidump_dir(struct minidump_dir_t *minidump_dir, FILE *in)
{
  minidump_dir->stream_type = read_uint32(in);
  minidump_dir->len_data = read_uint32(in);
  minidump_dir->ofs_data = read_uint32(in);

  return 0;
}

int read_minidump_location_desc(struct minidump_location_desc_t *minidump_location_desc, FILE *in)
{
  minidump_location_desc->len_data = read_uint32(in);
  minidump_location_desc->ofs_data = read_uint32(in);

  return 0;
}

void print_minidump_header(struct minidump_header_t *minidump_header)
{
  printf(" -------- MiniDump header -------\n");
  printf("     magic1: %.4s\n", minidump_header->magic1);
  printf("     magic2: 0x%02x 0x%02x\n", minidump_header->magic2[0],
                                         minidump_header->magic2[1]);
  printf("    version: %d\n", minidump_header->version);
  printf("num_streams: %d\n", minidump_header->num_streams);
  printf("ofs_streams: %d\n", minidump_header->ofs_streams);
  printf("   checksum: %d\n", minidump_header->checksum);
  printf("  timestamp: %d\n", minidump_header->timestamp);
  printf("      flags: 0x%lx\n", minidump_header->flags);
}

void print_minidump_dir(struct minidump_dir_t *minidump_dir, int index)
{
  printf(" -------- MiniDump dir %d -------\n", index);
  printf("stream_type: %d (%s)\n",
    minidump_dir->stream_type,
    get_minidump_stream_type(minidump_dir->stream_type));
  printf("   len_data: %u\n", minidump_dir->len_data);
  printf("   ofs_data: %u\n", minidump_dir->ofs_data);
}

#if 0
static void print_minidump_location_descriptor(FILE *in)
{
  //uint32_t n;
  uint32_t len_data = read_uint32(in);
  uint32_t ofs_data = read_uint32(in);

  printf("        len_data: %u\n", len_data);
  printf("        ofs_data: 0x%u\n", ofs_data);

  long marker = ftell(in);
  fseek(in, ofs_data, SEEK_SET);

  for (n = 0; n < len_data; n++)
  {
    if ((n % 16) == 0) { printf("\n"); }
    printf(" %02x", read_uint8(in));
  }

  printf("\n");

  fseek(in, marker, SEEK_SET);
}
#endif

static void print_minidump_string(FILE *in, uint32_t offset)
{
  uint32_t n;

  if (offset == 0)
  {
    printf("length=0\n");
    return;
  }

  long marker = ftell(in);
  fseek(in, offset, SEEK_SET);

  uint32_t length = read_uint32(in);

  printf(",length=%d\n", length);

  for (n = 0; n < length; n += 2)
  {
    uint16_t c = read_uint16(in);

    putchar(c);
  }

  fseek(in, marker, SEEK_SET);

  printf("\n");
}

static void print_minidump_memory_descriptor(FILE *in)
{
  struct minidump_location_desc_t minidump_location_desc;

  printf("    start_of_memory_range: 0x%" PRIx64 "\n", read_uint64(in));
  printf("                   memory:\n");

  read_minidump_location_desc(&minidump_location_desc, in);

  printf("              [ len_data: %u, ofs_data: %u ]\n",
     minidump_location_desc.len_data,
     minidump_location_desc.ofs_data);
}

static void print_minidump_thread_context_x86(FILE *in)
{
  int n;

  printf("    context_flags: 0x%x\n", read_uint32(in));
  printf("    dr0: 0x%08x  dr1: 0x%08x  dr2: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in));
  printf("    dr3: 0x%08x  dr6: 0x%08x  dr7: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in));

  // Fsave flags.
  printf("    fcw: 0x%04x  reserved_1: 0x%04x\n",
    read_uint16(in), read_uint16(in));
  printf("    fsw: 0x%04x  reserved_2: 0x%04x\n",
    read_uint16(in), read_uint16(in));
  printf("    ftw: 0x%04x  reserved_3: 0x%04x\n",
    read_uint16(in), read_uint16(in));
  printf(" fpu_ip: 0x%08x  fpu_cs: 0x%04x  fop: 0x%04x\n",
    read_uint32(in), read_uint16(in), read_uint16(in));
  printf(" fpu_dp: 0x%08x  fpu_ds: 0x%04x  reserved_4: 0x%04x\n",
    read_uint32(in), read_uint16(in), read_uint16(in));

  for (n = 0; n < 8; n++)
  {
    printf("  st%d: 0x%08" PRIx64 " 0x%02x 0x%02x", n,
      read_uint64(in), read_uint8(in), read_uint8(in));
    if ((n & 1) == 1) { printf("\n"); }
  }

  printf(" spare_0: 0x%08x\n", read_uint32(in));

#if 0
  printf("     gs: 0x%08x   fs: 0x%08x   es: 0x%08x   ds: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("    edi: 0x%08x  esi: 0x%08x  ebx: 0x%08x  edx: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("    ecx: 0x%08x  eax: 0x%08x  ebp: 0x%08x  eip: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("     cs: 0x%08x eflg: 0x%08x  esp: 0x%08x   ss: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
#endif

  // This section seems to be undocumented.  I got some information
  // from Chromium's Crashpad, but their information seems to be wrong.
  // I got these values by taking their information, writing a program
  // in VisualStudio 2015 that in an _asm section sets an x87 float
  // register, xmm1, and ecx and made it dereference a NULL pointer.
  // In VisualStudio I dumped all the registers and rearranged these
  // based on those values.  Unfortunately, ds, gs, es, and ss were
  // the same so I'm not sure if those are mixed up.  Actually, it
  // looks like from left to right it was a mirror image.
  printf("     ds: 0x%08x   es: 0x%08x   fs: 0x%08x   gs: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("    edx: 0x%08x  ebx: 0x%08x  esi: 0x%08x  edi: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("    eip: 0x%08x  ebp: 0x%08x  eax: 0x%08x  ecx: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  printf("     ss: 0x%08x  esp: 0x%08x eflg: 0x%08x   cs: 0x%08x\n",
    read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));

  printf("    fcw: 0x%04x  fsw: 0x%04x ftw: 0x%02x reserved_1: 0x%02x\n",
    read_uint16(in), read_uint16(in), read_uint8(in), read_uint8(in));
  printf("    fop: 0x%04x  fpu_ip: 0x%08x\n",
    read_uint16(in), read_uint32(in));
  printf(" fpu_cs: 0x%04x  reserved_2: 0x%08x\n",
    read_uint16(in), read_uint16(in));
  printf(" fpu_dp: 0x%08x  fpu_ds: 0x%04x  reserved_3: 0x%04d\n",
    read_uint32(in), read_uint16(in), read_uint16(in));
  printf("  mxcsr: 0x%08x  mxcsr_mask: 0x%08x\n",
    read_uint32(in), read_uint32(in));

  for (n = 0; n < 8; n++)
  {
    printf("  mm%d: 0x%016" PRIx64, n, read_uint64(in));

    read_uint64(in);

    if ((n % 2) == 1) { printf("\n"); }
  }

  for (n = 0; n < 8; n++)
  {
    printf(" xmm%d: 0x%08x 0x%08x 0x%08x 0x%08x\n", n,
      read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  }

  printf("\n");
}

static void print_minidump_thread_context_x86_64(FILE *in)
{
  int n;

  printf("    context_flags: 0x%x\n", read_uint32(in));
  printf("           mx_csr: 0x%x\n", read_uint32(in));

  read_uint64(in);
  read_uint64(in);
  read_uint64(in);
  read_uint64(in);
  read_uint64(in);
  read_uint64(in);

  printf("     fs: 0x%04x   es: 0x%04x   ds: 0x%04x   cs: 0x%04x\n",
   read_uint16(in), read_uint16(in), read_uint16(in), read_uint16(in));
  printf("          eflags: 0x%08x   ss: 0x%04x   gs: 0x%04x\n",
   read_uint32(in), read_uint16(in), read_uint16(in));

  printf("    dr0: 0x%016" PRIx64 "   dr1: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    dr2: 0x%016" PRIx64 "   dr3: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    dr6: 0x%016" PRIx64 "   dr7: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));

  printf("    rcx: 0x%016" PRIx64 "   rax: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    rbx: 0x%016" PRIx64 "   rdx: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    rbp: 0x%016" PRIx64 "   rsp: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    rdi: 0x%016" PRIx64 "   rsi: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("     r9: 0x%016" PRIx64 "    r8: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    r11: 0x%016" PRIx64 "   r10: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    r13: 0x%016" PRIx64 "   r12: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    r15: 0x%016" PRIx64 "   r14: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("    rip: 0x%016" PRIx64 "\n", read_uint64(in));

  // Fxsave
  printf("    fcw: 0x%04x  fsw: 0x%04x  ftw: 0x%04x  reserved_1: 0x%04x\n",
    read_uint16(in), read_uint16(in), read_uint8(in), read_uint8(in));
  printf("    fop: 0x%04x\n", read_uint16(in));
  printf("    fpu_ip_64: 0x%016" PRIx64 "   fpu_dp_64: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("     mxcsr: 0x%x  mxcsr_mask: 0x%x\n",
    read_uint32(in), read_uint32(in));

  for (n = 0; n < 8; n++)
  {
    printf("  mm%d: 0x%016" PRIx64, n, read_uint64(in));

    read_uint64(in);

    if ((n % 2) == 1) { printf("\n"); }
  }

  for (n = 0; n < 16; n++)
  {
    printf(" xmm%d: 0x%08x 0x%08x 0x%08x 0x%08x\n", n,
      read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  }

  for (n = 0; n < 26; n++)
  {
    printf(" ymm%d: 0x%08x 0x%08x 0x%08x 0x%08x\n", n,
      read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
    printf("        0x%08x 0x%08x 0x%08x 0x%08x\n",
      read_uint32(in), read_uint32(in), read_uint32(in), read_uint32(in));
  }

  printf("  p1_home: 0x%016" PRIx64 "  p2_home: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("  p3_home: 0x%016" PRIx64 "  p4_home: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("  p5_home: 0x%016" PRIx64 "  p6_home: 0x%016" PRIx64 "\n",
   read_uint64(in), read_uint64(in));
  printf("            debug_control: 0x%016" PRIx64 "\n", read_uint64(in));
  printf("       last_branch_to_rip: 0x%016" PRIx64 "\n", read_uint64(in));
  printf("     last_branch_from_rip: 0x%016" PRIx64 "\n", read_uint64(in));
  printf("    last_exception_to_rip: 0x%016" PRIx64 "\n", read_uint64(in));
  printf("  last_exception_from_rip: 0x%016" PRIx64 "\n", read_uint64(in));
}

static void print_minidump_thread(FILE *in, uint32_t cpu_arch)
{
  struct minidump_location_desc_t minidump_location_desc;

  read_minidump_location_desc(&minidump_location_desc, in);

  printf("              [ len_data: %u, ofs_data: %u ]\n",
     minidump_location_desc.len_data,
     minidump_location_desc.ofs_data);

  long marker = ftell(in);
  fseek(in, minidump_location_desc.ofs_data, SEEK_SET);

  if (cpu_arch == 0)
  {
    print_minidump_thread_context_x86(in);
  }
    else
  {
    print_minidump_thread_context_x86_64(in);
  }

  fseek(in, marker, SEEK_SET);
}

void print_minidump_thread_list(FILE *in, uint32_t cpu_arch)
{
  uint32_t n;
  uint32_t num_threads = read_uint32(in);

  printf("  - num_threads: %d\n", num_threads);

  for (n = 0; n < num_threads; n++)
  {
    printf("  -- Thread %d --\n", n);

    printf("        thread_id: %u\n", read_uint32(in));
    printf("    suspend_count: %u\n", read_uint32(in));
    printf("   priority_class: %u\n", read_uint32(in));
    printf("         priority: %u\n", read_uint32(in));
    printf("              teb: %" PRIx64 "\n", read_uint64(in));
    printf("            stack:\n");
    print_minidump_memory_descriptor(in);
    printf("   thread_context:\n");
    print_minidump_thread(in, cpu_arch);
  }

  printf("\n");
}

static void print_minidump_vs_fixed_file_info(FILE *in)
{
    printf("           Signature: 0x%x\n", read_uint32(in));
    printf("       StructVersion: %u\n", read_uint32(in));
    printf("       FileVersionMS: %u\n", read_uint32(in));
    printf("       FileVersionLS: %u\n", read_uint32(in));
    printf("    ProductVersionMS: %u\n", read_uint32(in));
    printf("    ProductVersionLS: %u\n", read_uint32(in));
    printf("       FileFlagsMask: %u\n", read_uint32(in));
    printf("           FileFlags: %u\n", read_uint32(in));
    printf("              FileOS: %u\n", read_uint32(in));
    printf("            FileType: %u\n", read_uint32(in));
    printf("         FileSubtype: %u\n", read_uint32(in));
    printf("          FileDateMS: %u\n", read_uint32(in));
    printf("          FileDateLS: %u\n", read_uint32(in));
}

void print_minidump_module_list(FILE *in)
{
  struct minidump_location_desc_t minidump_location_desc;
  uint32_t n;
  uint32_t num_modules = read_uint32(in);

  printf("  - num_modules: %d\n", num_modules);

  for (n = 0; n < num_modules; n++)
  {
    printf("  -- Module %d --\n", n);
    printf("    base_of_image: 0x%" PRIx64 "\n", read_uint64(in));
    printf("    size_of_image: %u\n", read_uint32(in));
    printf("         checksum: %u\n", read_uint32(in));
    printf("  time_date_stamp: %u\n", read_uint32(in));

    uint32_t module_name_rva = read_uint32(in);
    printf("      module_name: offset=%u", module_name_rva);
    print_minidump_string(in, module_name_rva);

    print_minidump_vs_fixed_file_info(in);

    read_minidump_location_desc(&minidump_location_desc, in);

    printf("        cv_record:\n");
    printf("              [ len_data: %u, ofs_data: %u ]\n",
       minidump_location_desc.len_data,
       minidump_location_desc.ofs_data);

    read_minidump_location_desc(&minidump_location_desc, in);

    printf("      misc_record:\n");
    printf("              [ len_data: %u, ofs_data: %u ]\n",
       minidump_location_desc.len_data,
       minidump_location_desc.ofs_data);

    printf("        reserved0: %" PRId64 "\n", read_uint64(in));
    printf("        reserved1: %" PRId64 "\n", read_uint64(in));
  }
}

void print_minidump_memory_list(FILE *in)
{
  uint64_t n;
  uint32_t size_of_headers = read_uint32(in);
  uint32_t size_of_entry = read_uint32(in);
  uint64_t number_of_entries = read_uint64(in);

  printf("   size_of_headers: %u\n", size_of_headers);
  printf("     size_of_entry: %u\n", size_of_entry);

  for (n = 0; n < number_of_entries; n++)
  {
    uint64_t marker = ftell(in);

    printf("  -- Memory Info %ld --\n", n);

    printf("      base_address: 0x%" PRIx64 "\n", read_uint64(in));
    printf("   allocation_base: 0x%" PRIx64 "\n", read_uint64(in));
    printf("allocation_protect: 0x%x\n", read_uint32(in));
    printf("      __alignment1: %d\n", read_uint32(in));
    printf("       region_size: %" PRId64 "\n", read_uint64(in));
    printf("             state: %d\n", read_uint32(in));
    printf("           protect: %d\n", read_uint32(in));
    printf("              type: %d\n", read_uint32(in));
    printf("      __alignment2: %d\n", read_uint32(in));

    uint64_t length = ftell(in) - marker;

    if (length < size_of_entry)
    {
      fseek(in, size_of_entry - marker, SEEK_CUR);
    }
  }
}

void print_minidump_exception(FILE *in)
{
  uint32_t n;

  printf("         thread_id: %d\n", read_uint32(in));
  printf("            unused: %d\n", read_uint32(in));
  printf("    exception_code: 0x%08x\n", read_uint32(in));
  printf("   exception_flags: 0x%08x\n", read_uint32(in));
  printf("  exception_record: 0x%" PRIx64 "\n", read_uint64(in));
  printf(" exception_address: 0x%" PRIx64 "\n", read_uint64(in));

  uint32_t number_parameters = read_uint32(in);
  printf("  number_paramters: %d\n", number_parameters);
  printf("            unused: %d\n", read_uint32(in));
  printf("       information:\n");

  for (n = 0; n < number_parameters; n++)
  {
    printf("      ---- %d ----\n", n);
    printf("            thread_id: %d\n", read_uint32(in));
    printf("    excpetion_pointer: %d\n", read_uint32(in));
    printf("      client_pointers: %d\n", read_uint32(in));
  }
}

void print_minidump_system_info(FILE *in)
{
  uint16_t cpu_arch = read_uint16(in);
  const char *cpu_arch_str = "???";

  switch (cpu_arch)
  {
    case 9: cpu_arch_str = "x64"; break;
    case 5: cpu_arch_str = "ARM"; break;
    case 6: cpu_arch_str = "Itanium"; break;
    case 0: cpu_arch_str = "x86"; break;
    case 0xffff: cpu_arch_str = "Unknown"; break;
  }

  printf("\n");
  printf("         cpu_arch: %u (%s)\n", cpu_arch, cpu_arch_str);
  printf("        cpu_level: %u\n", read_uint16(in));
  printf("     cpu_revision: %u\n", read_uint16(in));
  printf("         num_cpus: %u\n", read_uint8(in));
  printf("          os_type: %u\n", read_uint8(in));
  printf("     os_ver_major: %u\n", read_uint32(in));
  printf("     os_ver_minor: %u\n", read_uint32(in));
  printf("         os_build: %u\n", read_uint32(in));
  printf("      os_platform: %u\n", read_uint32(in));

  uint32_t ofs_service_pack = read_uint32(in);

  printf(" ofs_service_pack: %u", ofs_service_pack);
  print_minidump_string(in, ofs_service_pack);

  printf("    os_suite_mask: %u\n", read_uint16(in));
  printf("        reserved2: %u\n", read_uint16(in));
}

void print_minidump_memory64_list(FILE *in, struct memory_print_t *memory_print)
{
  uint64_t number_of_memory_ranges = read_uint64(in);
  uint64_t offset = read_uint64(in);
  uint64_t n;

  printf("\n");
  printf("  num_memory_ranges: %" PRId64 "\n", number_of_memory_ranges);
  printf("             offset: 0x%" PRIx64 "\n", offset);

  for (n = 0; n < number_of_memory_ranges; n++)
  {
    uint64_t start = read_uint64(in);
    uint64_t size = read_uint64(in);
    char mark = ' ';

    if (memory_print->start >= start &&
        memory_print->start < start + size)
    {
      memory_print->file_offset = offset + (memory_print->start - start);
      memory_print->region_start = start;
      memory_print->region_end = start + size - 1;
      mark = '*';
    }

    printf("         %c  0x%08" PRIx64 ": size=%" PRId64 "\n", mark, start, size);

    offset += size;
  }
}

void print_minidump_handle_data(FILE *in)
{
  uint32_t n;

  printf("\n");
  printf("     size_of_header: %u\n", read_uint32(in));

  uint32_t size_of_descriptor = read_uint32(in);
  uint32_t number_of_descriptors = read_uint32(in);

  printf(" size_of_descriptor: %u\n", size_of_descriptor);
  printf(" number_descriptors: %u\n", number_of_descriptors);
  printf("           reserved: %u\n", read_uint32(in));

  for (n = 0; n < number_of_descriptors; n++)
  {
    printf("       --- handle descriptor %d ---\n", n);
    long marker = ftell(in);

    printf("               handle: 0x%" PRIx64 "\n", read_uint64(in));

    uint32_t type_name_offset = read_uint32(in);
    uint32_t object_name_offset = read_uint32(in);

    printf("            type_name: offset=%d,", type_name_offset);
    print_minidump_string(in, type_name_offset);

    printf("          object_name: offset=%d,", object_name_offset);
    print_minidump_string(in, object_name_offset);

    printf("           attributes: 0x%x\n", read_uint32(in));
    printf("       granted_access: 0x%x\n", read_uint32(in));
    printf("         handle_count: %u\n", read_uint32(in));
    printf("        pointer_count: %u\n", read_uint32(in));

    if (size_of_descriptor > 32)
    {
      printf("          object_info: offset=%u\n", read_uint32(in));
      printf("             reserved: %u\n", read_uint32(in));
    }

    long end = ftell(in);

    if (end - marker < size_of_descriptor)
    {
      fseek(in, end - marker - size_of_descriptor, SEEK_CUR);
    }
  }
}

void print_minidump_misc_info(FILE *in)
{
  printf("\n");
  printf("           len_info: %u\n", read_uint32(in));
  printf("             flags1: %u\n", read_uint32(in));
  printf("         process_id: %u\n", read_uint32(in));
  printf("process_create_time: %u\n", read_uint32(in));
  printf("  process_user_time: %u\n", read_uint32(in));
  printf("process_kernel_time: %u\n", read_uint32(in));
  printf("        cpu_max_mhz: %u\n", read_uint32(in));
  printf("        cpu_cur_mhz: %u\n", read_uint32(in));
  printf("      cpu_limit_mhz: %u\n", read_uint32(in));
  printf(" cpu_max_idle_state: %u\n", read_uint32(in));
  printf(" cpu_cur_idle_state: %u\n", read_uint32(in));
}

void print_minidump_thread_info_list(FILE *in)
{
  printf("\n");
  printf("     size_of_header: %u\n", read_uint32(in));
  printf("      size_of_entry: %u\n", read_uint32(in));
  printf("  number_of_entries: %u\n", read_uint32(in));
}

