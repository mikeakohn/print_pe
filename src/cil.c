/*

print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "cil.h"
#include "fileio.h"
#include "table_cil_instr.h"

void disasm_cil(uint8_t *buffer, int len)
{
  int pc = 0;

  while (pc < len)
  {
    switch (buffer[pc])
    {
      case 0: // 0x00: // nop(0x00)
      case 1: // 0x01: // break(0x01)
      case 2: // 0x02: // ldarg.0(0x02)
      case 3: // 0x03: // ldarg.1(0x03)
      case 4: // 0x04: // ldarg.2(0x04)
      case 5: // 0x05: // ldarg.3(0x05)
      case 6: // 0x06: // ldloc.0(0x06)
      case 7: // 0x07: // ldloc.1(0x07)
      case 8: // 0x08: // ldloc.2(0x08)
      case 9: // 0x09: // ldloc.3(0x09)
      case 10: // 0x0A: // stloc.0(0x0a)
      case 11: // 0x0B: // stloc.1(0x0b)
      case 12: // 0x0C: // stloc.2(0x0c)
      case 13: // 0x0D: // stloc.3(0x0d)
      case 14: // 0x0E: // ldarg.s(0x0e)
      case 15: // 0x0F: // ldarga.s(0x0f)
      case 16: // 0x10: // starg.s(0x10)
      case 17: // 0x11: // ldloc.s(0x11)
      case 18: // 0x12: // ldloca.s(0x12)
      case 19: // 0x13: // stloc.s(0x13)
      case 20: // 0x14: // ldnull(0x14)
      case 21: // 0x15: // ldc.i4.m1(0x15)
      case 22: // 0x16: // ldc.i4.0(0x16)
      case 23: // 0x17: // ldc.i4.1(0x17)
      case 24: // 0x18: // ldc.i4.2(0x18)
      case 25: // 0x19: // ldc.i4.3(0x19)
      case 26: // 0x1A: // ldc.i4.4(0x1a)
      case 27: // 0x1B: // ldc.i4.5(0x1b)
      case 28: // 0x1C: // ldc.i4.6(0x1c)
      case 29: // 0x1D: // ldc.i4.7(0x1d)
      case 30: // 0x1E: // ldc.i4.8(0x1e)
      case 31: // 0x1F: // ldc.i4.s(0x1f)
      case 32: // 0x20: // ldc.i4(0x20)
      case 33: // 0x21: // ldc.i8(0x21)
      case 34: // 0x22: // ldc.r4(0x22)
      case 35: // 0x23: // ldc.r8(0x23)
      case 37: // 0x25: // dup(0x25)
      case 38: // 0x26: // pop(0x26)
      case 39: // 0x27: // jmp(0x27)
      case 40: // 0x28: // call(0x28)
      case 41: // 0x29: // calli(0x29)
      case 42: // 0x2A: // ret(0x2a)
      case 43: // 0x2B: // br.s(0x2b)
      case 44: // 0x2C: // brfalse.s(0x2c)
      case 45: // 0x2D: // brtrue.s(0x2d)
      case 46: // 0x2E: // beq.s(0x2e)
      case 47: // 0x2F: // bge.s(0x2f)
      case 48: // 0x30: // bgt.s(0x30)
      case 49: // 0x31: // ble.s(0x31)
      case 50: // 0x32: // blt.s(0x32)
      case 51: // 0x33: // bne.un.s(0x33)
      case 52: // 0x34: // bge.un.s(0x34)
      case 53: // 0x35: // bgt.un.s(0x35)
      case 54: // 0x36: // ble.un.s(0x36)
      case 55: // 0x37: // blt.un.s(0x37)
      case 56: // 0x38: // br(0x38)
      case 57: // 0x39: // brfalse(0x39)
      case 58: // 0x3A: // brtrue(0x3a)
      case 59: // 0x3B: // beq(0x3b)
      case 60: // 0x3C: // bge(0x3c)
      case 61: // 0x3D: // bgt(0x3d)
      case 62: // 0x3E: // ble(0x3e)
      case 63: // 0x3F: // blt(0x3f)
      case 64: // 0x40: // bne.un(0x40)
      case 65: // 0x41: // bge.un(0x41)
      case 66: // 0x42: // bgt.un(0x42)
      case 67: // 0x43: // ble.un(0x43)
      case 68: // 0x44: // blt.un(0x44)
      case 69: // 0x45: // switch(0x45)
      case 70: // 0x46: // ldind.i1(0x46)
      case 71: // 0x47: // ldind.u1(0x47)
      case 72: // 0x48: // ldind.i2(0x48)
      case 73: // 0x49: // ldind.u2(0x49)
      case 74: // 0x4A: // ldind.i4(0x4a)
      case 75: // 0x4B: // ldind.u4(0x4b)
      case 76: // 0x4C: // ldind.i8(0x4c)
      case 77: // 0x4D: // ldind.i(0x4d)
      case 78: // 0x4E: // ldind.r4(0x4e)
      case 79: // 0x4F: // ldind.r8(0x4f)
      case 80: // 0x50: // ldind.ref(0x50)
      case 81: // 0x51: // stind.ref(0x51)
      case 82: // 0x52: // stind.i1(0x52)
      case 83: // 0x53: // stind.i2(0x53)
      case 84: // 0x54: // stind.i4(0x54)
      case 85: // 0x55: // stind.i8(0x55)
      case 86: // 0x56: // stind.r4(0x56)
      case 87: // 0x57: // stind.r8(0x57)
      case 88: // 0x58: // add(0x58)
      case 89: // 0x59: // sub(0x59)
      case 90: // 0x5A: // mul(0x5a)
      case 91: // 0x5B: // div(0x5b)
      case 92: // 0x5C: // div.un(0x5c)
      case 93: // 0x5D: // rem(0x5d)
      case 94: // 0x5E: // rem.un(0x5e)
      case 95: // 0x5F: // and(0x5f)
      case 96: // 0x60: // or(0x60)
      case 97: // 0x61: // xor(0x61)
      case 98: // 0x62: // shl(0x62)
      case 99: // 0x63: // shr(0x63)
      case 100: // 0x64: // shr.un(0x64)
      case 101: // 0x65: // neg(0x65)
      case 102: // 0x66: // not(0x66)
      case 103: // 0x67: // conv.i1(0x67)
      case 104: // 0x68: // conv.i2(0x68)
      case 105: // 0x69: // conv.i4(0x69)
      case 106: // 0x6A: // conv.i8(0x6a)
      case 107: // 0x6B: // conv.r4(0x6b)
      case 108: // 0x6C: // conv.r8(0x6c)
      case 109: // 0x6D: // conv.u4(0x6d)
      case 110: // 0x6E: // conv.u8(0x6e)
      case 111: // 0x6F: // callvirt(0x6f)
      case 112: // 0x70: // cpobj(0x70)
      case 113: // 0x71: // ldobj(0x71)
      case 114: // 0x72: // ldstr(0x72)
      case 115: // 0x73: // newobj(0x73)
      case 116: // 0x74: // castclass(0x74)
      case 117: // 0x75: // isinst(0x75)
      case 118: // 0x76: // conv.r.un(0x76)
      case 121: // 0x79: // unbox(0x79)
      case 122: // 0x7A: // throw(0x7a)
      case 123: // 0x7B: // ldfld(0x7b)
      case 124: // 0x7C: // ldflda(0x7c)
      case 125: // 0x7D: // stfld(0x7d)
      case 126: // 0x7E: // ldsfld(0x7e)
      case 127: // 0x7F: // ldsflda(0x7f)
      case 128: // 0x80: // stsfld(0x80)
      case 129: // 0x81: // stobj(0x81)
      case 130: // 0x82: // conv.ovf.i1.un(0x82)
      case 131: // 0x83: // conv.ovf.i2.un(0x83)
      case 132: // 0x84: // conv.ovf.i4.un(0x84)
      case 133: // 0x85: // conv.ovf.i8.un(0x85)
      case 134: // 0x86: // conv.ovf.u1.un(0x86)
      case 135: // 0x87: // conv.ovf.u2.un(0x87)
      case 136: // 0x88: // conv.ovf.u4.un(0x88)
      case 137: // 0x89: // conv.ovf.u8.un(0x89)
      case 138: // 0x8A: // conv.ovf.i.un(0x8a)
      case 139: // 0x8B: // conv.ovf.u.un(0x8b)
      case 140: // 0x8C: // box(0x8c)
      case 141: // 0x8D: // newarr(0x8d)
      case 142: // 0x8E: // ldlen(0x8e)
      case 143: // 0x8F: // ldelema(0x8f)
      case 144: // 0x90: // ldelem.i1(0x90)
      case 145: // 0x91: // ldelem.u1(0x91)
      case 146: // 0x92: // ldelem.i2(0x92)
      case 147: // 0x93: // ldelem.u2(0x93)
      case 148: // 0x94: // ldelem.i4(0x94)
      case 149: // 0x95: // ldelem.u4(0x95)
      case 150: // 0x96: // ldelem.i8(0x96)
      case 151: // 0x97: // ldelem.i(0x97)
      case 152: // 0x98: // ldelem.r4(0x98)
      case 153: // 0x99: // ldelem.r8(0x99)
      case 154: // 0x9A: // ldelem.ref(0x9a)
      case 155: // 0x9B: // stelem.i(0x9b)
      case 156: // 0x9C: // stelem.i1(0x9c)
      case 157: // 0x9D: // stelem.i2(0x9d)
      case 158: // 0x9E: // stelem.i4(0x9e)
      case 159: // 0x9F: // stelem.i8(0x9f)
      case 160: // 0xA0: // stelem.r4(0xa0)
      case 161: // 0xA1: // stelem.r8(0xa1)
      case 162: // 0xA2: // stelem.ref(0xa2)
      case 163: // 0xA3: // ldelem(0xa3)
      case 164: // 0xA4: // stelem(0xa4)
      case 165: // 0xA5: // unbox.any(0xa5)
      case 179: // 0xB3: // conv.ovf.i1(0xb3)
      case 180: // 0xB4: // conv.ovf.u1(0xb4)
      case 181: // 0xB5: // conv.ovf.i2(0xb5)
      case 182: // 0xB6: // conv.ovf.u2(0xb6)
      case 183: // 0xB7: // conv.ovf.i4(0xb7)
      case 184: // 0xB8: // conv.ovf.u4(0xb8)
      case 185: // 0xB9: // conv.ovf.i8(0xb9)
      case 186: // 0xBA: // conv.ovf.u8(0xba)
      case 194: // 0xC2: // refanyval(0xc2)
      case 195: // 0xC3: // ckfinite(0xc3)
      case 198: // 0xC6: // mkrefany(0xc6)
      case 208: // 0xD0: // ldtoken(0xd0)
      case 209: // 0xD1: // conv.u2(0xd1)
      case 210: // 0xD2: // conv.u1(0xd2)
      case 211: // 0xD3: // conv.i(0xd3)
      case 212: // 0xD4: // conv.ovf.i(0xd4)
      case 213: // 0xD5: // conv.ovf.u(0xd5)
      case 214: // 0xD6: // add.ovf(0xd6)
      case 215: // 0xD7: // add.ovf.un(0xd7)
      case 216: // 0xD8: // mul.ovf(0xd8)
      case 217: // 0xD9: // mul.ovf.un(0xd9)
      case 218: // 0xDA: // sub.ovf(0xda)
      case 219: // 0xDB: // sub.ovf.un(0xdb)
      case 220: // 0xDC: // endfinally(0xdc)
      case 221: // 0xDD: // leave(0xdd)
      case 222: // 0xDE: // leave.s(0xde)
      case 223: // 0xDF: // stind.i(0xdf)
      case 224: // 0xE0: // conv.u(0xe0)
      case 254:
        switch (buffer[pc + 1])
        {
          case 0: // 0x00: // arglist(0x00)
          case 1: // 0x01: // ceq(0x01)
          case 2: // 0x02: // cgt(0x02)
          case 3: // 0x03: // cgt.un(0x03)
          case 4: // 0x04: // clt(0x04)
          case 5: // 0x05: // clt.un(0x05)
          case 6: // 0x06: // ldftn(0x06)
          case 7: // 0x07: // ldvirtftn(0x07)
          case 9: // 0x09: // ldarg(0x09)
          case 10: // 0x0A: // ldarga(0x0a)
          case 11: // 0x0B: // starg(0x0b)
          case 12: // 0x0C: // ldloc(0x0c)
          case 13: // 0x0D: // ldloca(0x0d)
          case 14: // 0x0E: // stloc(0x0e)
          case 15: // 0x0F: // localloc(0x0f)
          case 17: // 0x11: // endfilter(0x11)
          case 18: // 0x12: // unaligned.(0x12)
          case 19: // 0x13: // volatile.(0x13)
          case 20: // 0x14: // tail.(0x14)
          case 21: // 0x15: // Initobj(0x15)
          case 22: // 0x16: // constrained.(0x16)
          case 23: // 0x17: // cpblk(0x17)
          case 24: // 0x18: // initblk(0x18)
          case 25: // 0x19: // no.(0x19)
          case 26: // 0x1A: // rethrow(0x1a)
          case 28: // 0x1C: // sizeof(0x1c)
          case 29: // 0x1D: // Refanytype(0x1d)
          case 30: // 0x1E: // readonly.(0x1e)
          default:
            printf("Unknown opcode 0x%02x 0x%02x\n", buffer[pc], buffer[pc + 1]);
            break;
        }

      default:
        printf("Unknown opcode 0x%02x\n", buffer[pc]);
        break;
    }

    if (buffer[pc] == 0xfe)
    {
      pc += table_cil_instr_fe[buffer[pc]+1].size;
    }
      else
    {
      pc += table_cil_instr[buffer[pc]].size;
    }
  }
}

int read_clr_header(FILE *in, struct _clr_header *clr_header, int address, int size, struct section_header_t *section_header)
{
  long marker;
  uint32_t virtual_address, raw_ptr;

  virtual_address = section_header->VirtualAddress;
  raw_ptr = section_header->PointerToRawData;

  address = (address - virtual_address) + raw_ptr;

  marker = ftell(in);
  fseek(in, address, SEEK_SET);

  clr_header->header_size = read_uint32(in);
  clr_header->major_runtime_version = read_uint16(in);
  clr_header->minor_runtime_version = read_uint16(in);
  clr_header->meta_data_directory_address = read_uint32(in);
  clr_header->meta_data_directory_size = read_uint32(in);
  clr_header->flags = read_uint32(in);
  clr_header->entry_point_token = read_uint32(in);
  clr_header->resources_directory_address = read_uint32(in);
  clr_header->resources_directory_size = read_uint32(in);
  clr_header->strong_name_signature_address = read_uint32(in);
  clr_header->strong_name_signature_size = read_uint32(in);
  clr_header->v_table_fixups_address = read_uint32(in);
  clr_header->v_table_fixups_size = read_uint32(in);
  clr_header->export_address_table_jump_address = read_uint32(in);
  clr_header->export_address_table_jump_size = read_uint32(in);
  clr_header->managed_native_header_address = read_uint32(in);
  clr_header->managed_native_header_size = read_uint32(in);

  fseek(in, marker, SEEK_SET);

  return 0;
}

void print_clr_header(struct _clr_header *clr_header)
{
  printf("                Header Size: %d\n", clr_header->header_size);
  printf("      Major Runtime Version: %d\n", clr_header->major_runtime_version);
  printf("      Minor Runtime Version: %d\n", clr_header->minor_runtime_version);
  printf("          MetaData Dir Addr: %d\n", clr_header->meta_data_directory_address);
  printf("          MetaData Dir Size: %d\n", clr_header->meta_data_directory_size);
  printf("                      Flags: %d\n", clr_header->flags);
  printf("          Entry Point Token: %d\n", clr_header->entry_point_token);
  printf("         Resources Dir Addr: %d\n", clr_header->resources_directory_address);
  printf("         Resources Dir Size: %d\n", clr_header->resources_directory_size);
  printf("       Strong Name Sig Addr: %d\n", clr_header->strong_name_signature_address);
  printf("       Strong Name Sig Size: %d\n", clr_header->strong_name_signature_size);
  printf("         VTable Fixups Addr: %d\n", clr_header->v_table_fixups_address);
  printf("         VTable Fixups Size: %d\n", clr_header->v_table_fixups_size);
  printf("Export Addr Table Jump Addr: %d\n", clr_header->export_address_table_jump_address);
  printf("Export Addr Table Jump Size: %d\n", clr_header->export_address_table_jump_size);
  printf("    Mngd Native Header Addr: %d\n", clr_header->managed_native_header_address);
  printf("    Mngd Native Header Size: %d\n", clr_header->managed_native_header_size);
}


