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

#include "table_cil_instr.h"

struct _table_cil_instr table_cil_instr[] =
{
  { "nop", 1 }, // 0x00
  { "break", 1 }, // 0x01
  { "ldarg.0", 1 }, // 0x02
  { "ldarg.1", 1 }, // 0x03
  { "ldarg.2", 1 }, // 0x04
  { "ldarg.3", 1 }, // 0x05
  { "ldloc.0", 1 }, // 0x06
  { "ldloc.1", 1 }, // 0x07
  { "ldloc.2", 1 }, // 0x08
  { "ldloc.3", 1 }, // 0x09
  { "stloc.0", 1 }, // 0x0a
  { "stloc.1", 1 }, // 0x0b
  { "stloc.2", 1 }, // 0x0c
  { "stloc.3", 1 }, // 0x0d
  { "ldarg.s", 1 }, // 0x0e
  { "ldarga.s", 1 }, // 0x0f
  { "starg.s", 1 }, // 0x10
  { "ldloc.s", 1 }, // 0x11
  { "ldloca.s", 1 }, // 0x12
  { "stloc.s", 1 }, // 0x13
  { "ldnull", 1 }, // 0x14
  { "ldc.i4.m1", 1 }, // 0x15
  { "ldc.i4.0", 1 }, // 0x16
  { "ldc.i4.1", 1 }, // 0x17
  { "ldc.i4.2", 1 }, // 0x18
  { "ldc.i4.3", 1 }, // 0x19
  { "ldc.i4.4", 1 }, // 0x1a
  { "ldc.i4.5", 1 }, // 0x1b
  { "ldc.i4.6", 1 }, // 0x1c
  { "ldc.i4.7", 1 }, // 0x1d
  { "ldc.i4.8", 1 }, // 0x1e
  { "ldc.i4.s", 1 }, // 0x1f
  { "ldc.i4", 1 }, // 0x20
  { "ldc.i8", 1 }, // 0x21
  { "ldc.r4", 1 }, // 0x22
  { "ldc.r8", 1 }, // 0x23
  { "???", 1 }, // 0x24
  { "dup", 1 }, // 0x25
  { "pop", 1 }, // 0x26
  { "jmp", 1 }, // 0x27
  { "call", 1 }, // 0x28
  { "calli", 1 }, // 0x29
  { "ret", 1 }, // 0x2a
  { "br.s", 1 }, // 0x2b
  { "brfalse.s", 1 }, // 0x2c
  { "brtrue.s", 1 }, // 0x2d
  { "beq.s", 1 }, // 0x2e
  { "bge.s", 1 }, // 0x2f
  { "bgt.s", 1 }, // 0x30
  { "ble.s", 1 }, // 0x31
  { "blt.s", 1 }, // 0x32
  { "bne.un.s", 1 }, // 0x33
  { "bge.un.s", 1 }, // 0x34
  { "bgt.un.s", 1 }, // 0x35
  { "ble.un.s", 1 }, // 0x36
  { "blt.un.s", 1 }, // 0x37
  { "br", 1 }, // 0x38
  { "brfalse", 1 }, // 0x39
  { "brtrue", 1 }, // 0x3a
  { "beq", 1 }, // 0x3b
  { "bge", 1 }, // 0x3c
  { "bgt", 1 }, // 0x3d
  { "ble", 1 }, // 0x3e
  { "blt", 1 }, // 0x3f
  { "bne.un", 1 }, // 0x40
  { "bge.un", 1 }, // 0x41
  { "bgt.un", 1 }, // 0x42
  { "ble.un", 1 }, // 0x43
  { "blt.un", 1 }, // 0x44
  { "switch", 1 }, // 0x45
  { "ldind.i1", 1 }, // 0x46
  { "ldind.u1", 1 }, // 0x47
  { "ldind.i2", 1 }, // 0x48
  { "ldind.u2", 1 }, // 0x49
  { "ldind.i4", 1 }, // 0x4a
  { "ldind.u4", 1 }, // 0x4b
  { "ldind.i8", 1 }, // 0x4c
  { "ldind.i", 1 }, // 0x4d
  { "ldind.r4", 1 }, // 0x4e
  { "ldind.r8", 1 }, // 0x4f
  { "ldind.ref", 1 }, // 0x50
  { "stind.ref", 1 }, // 0x51
  { "stind.i1", 1 }, // 0x52
  { "stind.i2", 1 }, // 0x53
  { "stind.i4", 1 }, // 0x54
  { "stind.i8", 1 }, // 0x55
  { "stind.r4", 1 }, // 0x56
  { "stind.r8", 1 }, // 0x57
  { "add", 1 }, // 0x58
  { "sub", 1 }, // 0x59
  { "mul", 1 }, // 0x5a
  { "div", 1 }, // 0x5b
  { "div.un", 1 }, // 0x5c
  { "rem", 1 }, // 0x5d
  { "rem.un", 1 }, // 0x5e
  { "and", 1 }, // 0x5f
  { "or", 1 }, // 0x60
  { "xor", 1 }, // 0x61
  { "shl", 1 }, // 0x62
  { "shr", 1 }, // 0x63
  { "shr.un", 1 }, // 0x64
  { "neg", 1 }, // 0x65
  { "not", 1 }, // 0x66
  { "conv.i1", 1 }, // 0x67
  { "conv.i2", 1 }, // 0x68
  { "conv.i4", 1 }, // 0x69
  { "conv.i8", 1 }, // 0x6a
  { "conv.r4", 1 }, // 0x6b
  { "conv.r8", 1 }, // 0x6c
  { "conv.u4", 1 }, // 0x6d
  { "conv.u8", 1 }, // 0x6e
  { "callvirt", 1 }, // 0x6f
  { "cpobj", 1 }, // 0x70
  { "ldobj", 1 }, // 0x71
  { "ldstr", 1 }, // 0x72
  { "newobj", 1 }, // 0x73
  { "castclass", 1 }, // 0x74
  { "isinst", 1 }, // 0x75
  { "conv.r.un", 1 }, // 0x76
  { "???", 1 }, // 0x77
  { "???", 1 }, // 0x78
  { "unbox", 1 }, // 0x79
  { "throw", 1 }, // 0x7a
  { "ldfld", 1 }, // 0x7b
  { "ldflda", 1 }, // 0x7c
  { "stfld", 1 }, // 0x7d
  { "ldsfld", 1 }, // 0x7e
  { "ldsflda", 1 }, // 0x7f
  { "stsfld", 1 }, // 0x80
  { "stobj", 1 }, // 0x81
  { "conv.ovf.i1.un", 1 }, // 0x82
  { "conv.ovf.i2.un", 1 }, // 0x83
  { "conv.ovf.i4.un", 1 }, // 0x84
  { "conv.ovf.i8.un", 1 }, // 0x85
  { "conv.ovf.u1.un", 1 }, // 0x86
  { "conv.ovf.u2.un", 1 }, // 0x87
  { "conv.ovf.u4.un", 1 }, // 0x88
  { "conv.ovf.u8.un", 1 }, // 0x89
  { "conv.ovf.i.un", 1 }, // 0x8a
  { "conv.ovf.u.un", 1 }, // 0x8b
  { "box", 1 }, // 0x8c
  { "newarr", 1 }, // 0x8d
  { "ldlen", 1 }, // 0x8e
  { "ldelema", 1 }, // 0x8f
  { "ldelem.i1", 1 }, // 0x90
  { "ldelem.u1", 1 }, // 0x91
  { "ldelem.i2", 1 }, // 0x92
  { "ldelem.u2", 1 }, // 0x93
  { "ldelem.i4", 1 }, // 0x94
  { "ldelem.u4", 1 }, // 0x95
  { "ldelem.i8", 1 }, // 0x96
  { "ldelem.i", 1 }, // 0x97
  { "ldelem.r4", 1 }, // 0x98
  { "ldelem.r8", 1 }, // 0x99
  { "ldelem.ref", 1 }, // 0x9a
  { "stelem.i", 1 }, // 0x9b
  { "stelem.i1", 1 }, // 0x9c
  { "stelem.i2", 1 }, // 0x9d
  { "stelem.i4", 1 }, // 0x9e
  { "stelem.i8", 1 }, // 0x9f
  { "stelem.r4", 1 }, // 0xa0
  { "stelem.r8", 1 }, // 0xa1
  { "stelem.ref", 1 }, // 0xa2
  { "ldelem", 1 }, // 0xa3
  { "stelem", 1 }, // 0xa4
  { "unbox.any", 1 }, // 0xa5
  { "???", 1 }, // 0xa6
  { "???", 1 }, // 0xa7
  { "???", 1 }, // 0xa8
  { "???", 1 }, // 0xa9
  { "???", 1 }, // 0xaa
  { "???", 1 }, // 0xab
  { "???", 1 }, // 0xac
  { "???", 1 }, // 0xad
  { "???", 1 }, // 0xae
  { "???", 1 }, // 0xaf
  { "???", 1 }, // 0xb0
  { "???", 1 }, // 0xb1
  { "???", 1 }, // 0xb2
  { "conv.ovf.i1", 1 }, // 0xb3
  { "conv.ovf.u1", 1 }, // 0xb4
  { "conv.ovf.i2", 1 }, // 0xb5
  { "conv.ovf.u2", 1 }, // 0xb6
  { "conv.ovf.i4", 1 }, // 0xb7
  { "conv.ovf.u4", 1 }, // 0xb8
  { "conv.ovf.i8", 1 }, // 0xb9
  { "conv.ovf.u8", 1 }, // 0xba
  { "???", 1 }, // 0xbb
  { "???", 1 }, // 0xbc
  { "???", 1 }, // 0xbd
  { "???", 1 }, // 0xbe
  { "???", 1 }, // 0xbf
  { "???", 1 }, // 0xc0
  { "???", 1 }, // 0xc1
  { "refanyval", 1 }, // 0xc2
  { "ckfinite", 1 }, // 0xc3
  { "???", 1 }, // 0xc4
  { "???", 1 }, // 0xc5
  { "mkrefany", 1 }, // 0xc6
  { "???", 1 }, // 0xc7
  { "???", 1 }, // 0xc8
  { "???", 1 }, // 0xc9
  { "???", 1 }, // 0xca
  { "???", 1 }, // 0xcb
  { "???", 1 }, // 0xcc
  { "???", 1 }, // 0xcd
  { "???", 1 }, // 0xce
  { "???", 1 }, // 0xcf
  { "ldtoken", 1 }, // 0xd0
  { "conv.u2", 1 }, // 0xd1
  { "conv.u1", 1 }, // 0xd2
  { "conv.i", 1 }, // 0xd3
  { "conv.ovf.i", 1 }, // 0xd4
  { "conv.ovf.u", 1 }, // 0xd5
  { "add.ovf", 1 }, // 0xd6
  { "add.ovf.un", 1 }, // 0xd7
  { "mul.ovf", 1 }, // 0xd8
  { "mul.ovf.un", 1 }, // 0xd9
  { "sub.ovf", 1 }, // 0xda
  { "sub.ovf.un", 1 }, // 0xdb
  { "endfinally", 1 }, // 0xdc
  { "leave", 1 }, // 0xdd
  { "leave.s", 1 }, // 0xde
  { "stind.i", 1 }, // 0xdf
  { "conv.u", 1 }, // 0xe0
};

struct _table_cil_instr table_cil_instr_fe[] =
{
  { "arglist", 1 }, // 0xfe 0x00
  { "ceq", 1 }, // 0xfe 0x01
  { "cgt", 1 }, // 0xfe 0x02
  { "cgt.un", 1 }, // 0xfe 0x03
  { "clt", 1 }, // 0xfe 0x04
  { "clt.un", 1 }, // 0xfe 0x05
  { "ldftn", 1 }, // 0xfe 0x06
  { "ldvirtftn", 1 }, // 0xfe 0x07
  { "???", 1 }, // 0xfe 0x08
  { "ldarg", 1 }, // 0xfe 0x09
  { "ldarga", 1 }, // 0xfe 0x0a
  { "starg", 1 }, // 0xfe 0x0b
  { "ldloc", 1 }, // 0xfe 0x0c
  { "ldloca", 1 }, // 0xfe 0x0d
  { "stloc", 1 }, // 0xfe 0x0e
  { "localloc", 1 }, // 0xfe 0x0f
  { "???", 1 }, // 0xfe 0x10
  { "endfilter", 1 }, // 0xfe 0x11
  { "unaligned.", 1 }, // 0xfe 0x12
  { "volatile.", 1 }, // 0xfe 0x13
  { "tail.", 1 }, // 0xfe 0x14
  { "Initobj", 1 }, // 0xfe 0x15
  { "constrained.", 1 }, // 0xfe 0x16
  { "cpblk", 1 }, // 0xfe 0x17
  { "initblk", 1 }, // 0xfe 0x18
  { "no.", 1 }, // 0xfe 0x19
  { "rethrow", 1 }, // 0xfe 0x1a
  { "???", 1 }, // 0xfe 0x1b
  { "sizeof", 1 }, // 0xfe 0x1c
  { "Refanytype", 1 }, // 0xfe 0x1d
  { "readonly.", 1 }, // 0xfe 0x1e
};

