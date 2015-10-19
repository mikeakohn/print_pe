/*

print_pe - Copyright 2005-2015 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef PE_H
#define PE_H

#include <stdint.h>

struct funct_t
{
  char funct_name[1024];
  unsigned int funct_retvalue;
  long funct_ptr;
  long file_offset;
};

struct dos_header_t
{
  uint16_t e_magic;         // Magic number
  uint16_t e_cblp;          // Bytes on last page of file
  uint16_t e_cp;            // Pages in file
  uint16_t e_crlc;          // Relocations
  uint16_t e_cparhdr;       // Size of header in paragraphs
  uint16_t e_minalloc;      // Minimum extra paragraphs needed
  uint16_t e_maxalloc;      // Maximum extra paragraphs needed
  uint16_t e_ss;            // Initial (relative) SS value
  uint16_t e_sp;            // Initial SP value
  uint16_t e_csum;          // Checksum
  uint16_t e_ip;            // Initial IP value
  uint16_t e_cs;            // Initial (relative) CS value
  uint16_t e_lfarlc;        // File address of relocation table
  uint16_t e_ovno;          // Overlay number
  uint8_t e_res[8];             // Reserved words
  uint16_t e_oemid;         // OEM identifier (for e_oeminfo)
  uint16_t e_oeminfo;       // OEM information; e_oemid specific
  uint8_t e_res2[20];           // Reserved words
  int e_lfanew;                       // File address of new exe header
};

struct image_file_header_t
{
  uint16_t Machine;
  uint16_t NumberOfSections;
  uint32_t TimeDateStamp;
  uint32_t PointerToSymbolTable;
  uint32_t NumberOfSymbols;
  uint16_t SizeOfOptionalHeader;
  uint16_t Characteristics;
};

struct image_data_directory_t
{
  uint32_t VirtualAddress;
  uint32_t Size;
};

struct image_optional_header_t
{
  uint16_t Magic;
  uint8_t MajorLinkerVersion;
  uint8_t MinorLinkerVersion;
  uint32_t SizeOfCode;
  uint32_t SizeOfInitializedData;
  uint32_t SizeOfUninitializedData;
  uint32_t AddressOfEntryPoint;
  uint32_t BaseOfCode;
  uint32_t BaseOfData;
  uint32_t ImageBase;
  uint32_t SectionAlignment;
  uint32_t FileAlignment;
  uint16_t MajorOperatingSystemVersion;
  uint16_t MinorOperatingSystemVersion;
  uint16_t MajorImageVersion;
  uint16_t MinorImageVersion;
  uint16_t MajorSubsystemVersion;
  uint16_t MinorSubsystemVersion;
  uint32_t Reserved1;
  uint32_t SizeOfImage;
  uint32_t SizeOfHeaders;
  uint32_t CheckSum;
  uint16_t Subsystem;
  uint16_t DllCharacteristics;
  uint32_t SizeOfStackReserve;
  uint32_t SizeOfStackCommit;
  uint32_t SizeOfHeapReserve;
  uint32_t SizeOfHeapCommit;
  uint32_t LoaderFlags;
  uint32_t NumberOfRvaAndSizes;
  int DataDirectoryCount;
  int image_data_dir[32];
};

struct section_header_t
{
  char name[9];
  union
  {
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
  } Misc;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t NumberOfRelocations;
  uint16_t NumberOfLinenumbers;
  uint32_t Characteristics;
};

struct resource_dir_t
{
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint16_t NumberOfNamedEntries;
  uint16_t NumberOfIdEntries;
};

struct resource_dir_entry_t
{
  uint32_t Name;
  uint32_t OffsetToData;
  char RealName[256];
};

struct resource_data_t
{
  uint32_t OffsetToData;
  uint32_t Size;
  uint32_t CodePage;
  uint32_t Reserved;
};

struct import_dir_t
{
  uint32_t FunctionNameList;
  uint32_t TimeDateStamp;
  uint32_t ForwardChain;
  uint32_t ModuleName;
  uint32_t FunctionAddressList;
};

struct export_dir_t
{
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t Name;
  uint32_t Base;
  uint32_t NumberOfFunctions;
  uint32_t NumberOfNames;
  uint32_t AddressOfFunctions;
  uint32_t AddressOfNames;
  uint32_t AddressOfNameOrdinals;
};

int read_dos_header(FILE *in, struct dos_header_t *dos_header);
int print_dos_header(struct dos_header_t *dos_header);

int read_image_file_header(FILE *in, struct image_file_header_t *image_file_header);
int print_image_file_header(struct image_file_header_t *image_file_header);

int read_image_optional_header(FILE *in, struct image_optional_header_t *image_optional_header, int header_size);
int print_image_optional_header(struct image_optional_header_t *image_optional_header);

int read_section_header(FILE *in, struct section_header_t *section_header);
int print_section_header(struct section_header_t *section_header,int count);

int print_imports(FILE *in, int addr, int size, struct section_header_t *section_header);
int print_exports(FILE *in, int addr, int size, struct section_header_t *section_header, struct funct_t *funct);


int rip_binary(FILE *in, char *filename, int address, int size);
int read_unicode(FILE *in, int addr, char *s, int max_chars);

int parse_resource_dir(FILE *in, struct section_header_t *section_header, int offset, int level, int res_type);
int read_resource_data(FILE *in, int addr, struct resource_data_t *resource_data, int offset);
int read_resource_dir(FILE *in, int addr, struct resource_dir_t *resource_dir);
int read_resource_dir_entry(FILE *in, int addr, struct resource_dir_entry_t *resource_dir_entry, int offset);
int print_resource_data(struct resource_data_t *resource_data, int level);
int print_resource_type(int id, int level);
int print_resource_dir(struct resource_dir_t *resource_dir, int level);
int print_resource_dir_entry(struct resource_dir_entry_t *resource_dir_entry, int level);

#endif

