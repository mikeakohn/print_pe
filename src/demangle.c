/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "demangle.h"

static int demangle_append(
  const char *name,
  char *demangled,
  int offset,
  int length)
{
  while (*name != 0)
  {
    if (offset >= length -1) { return offset; }
    demangled[offset++] = *name;
    name++;
  }

  return offset;
}

static int demangle_insert_name(
  const char *name,
  char *demangled,
  int offset,
  int length)
{
  while (offset < length)
  {
    if (*name == 0 || *name == '@') { break; }

    if (*name == '?')
    {
      switch (name[1])
      {
        case '0': break;
        case '1': demangled[offset++] = '~'; break;
        case '2':
          offset = demangle_append("operator new ", demangled, offset, length);
          break;
        case '3':
          offset = demangle_append("operator delete ", demangled, offset, length);
          break;
        // FIXME: Add the rest of these.
        default: break;
      }

      name += 2;
      continue;
    }

    demangled[offset++] = *name;
    name++;
  }

  if (offset < length) { demangled[offset] = 0; }

  return offset;
}

static int demangle_params(
  const char *name,
  char *params,
  char *return_type,
  int offset,
  int length)
{
  int count = 0;

  if (offset < length) { params[offset++] = '('; }

  if (name[0] == 'Y' && name[1] == 'A')
  {
    name += 2;

    while (offset < length)
    {
      if (*name == 0 || *name == '@') { break; }

      const char *type = NULL;
      uint8_t underline = 0;

      if (*name == '_')
      {
        underline = 1;
        name++;
      }

      // FIXME: Not sure why some functions end with @Z and some just
      // have a Z.
      uint8_t last = 0;
      if (name[1] == '@') { last = 1; }
      if (name[1] == 'Z' && name[2] == 0) { last = 1; }

      if (offset < length && count != 0 && last == 0)
      {
        params[offset++] = ',';
      }

      if (underline == 0)
      {
        switch (*name)
        {
          case 'C': type = "int8_t";        break;
          case 'D': type = "char";          break;
          case 'E': type = "uint8_t";       break;
          case 'F': type = "int16_t";       break;
          case 'G': type = "uint16_t";      break;
          case 'H': type = "int";           break;
          case 'I': type = "uint32_t";      break;
          case 'J': type = "long";          break;
          case 'K': type = "unsigned long"; break;
          case 'M': type = "float";         break;
          case 'N': type = "double";        break;
          case 'O': type = "long double";   break;
          case 'P': type = "???";           break;
          case 'Q': type = "???";           break;
          case 'V': type = "???";           break;
          case 'X': type = "void";          break;
          case 'Z': type = "...";           break;
          default: type = "???";            break;
        }
      }
        else
      {
        switch (*name)
        {
          case '$': type = "__w64";             break;
          case 'D': type = "__int8";            break;
          case 'E': type = "uint8_t";           break;
          case 'F': type = "__int16";           break;
          case 'G': type = "uint16_t";          break;
          case 'H': type = "__int32";           break;
          case 'I': type = "uint32_t";          break;
          case 'J': type = "__int64";           break;
          case 'K': type = "uint64_t";          break;
          case 'L': type = "__int128";          break;
          case 'M': type = "unsigned __int128"; break;
          case 'N': type = "bool";              break;
          case 'O': type = "???";               break;
          case 'P': type = "???";               break;
          case 'Q': type = "???";               break;
          case 'S': type = "char_16_t";         break;
          case 'U': type = "char32_t";          break;
          case 'W': type = "wchar_t";           break;
          case 'X':
          case 'Z': type = "...";           break;
          default: type = "???";            break;
        }
      }

      if (type != NULL)
      {
        if (last == 0)
        {
          offset = demangle_insert_name(type, params, offset, length);
        }
          else
        {
          demangle_insert_name(type, return_type, 0, length);
        }
      }

      if (last == 1) { break; }

      name++;
      count++;
    }
  }
    else
  {
    if (offset < length) { params[offset++] = '?'; }
  }

  if (offset < length) { params[offset++] = ')'; }
  if (offset < length) { params[offset] = 0; }

  return offset;
}

static int demangle_name(
  const char *name,
  char *demangled,
  char *params,
  char *return_type,
  int offset,
  int length)
{
  const char *s = name;
  int ptr = 0;

  while (s[ptr] != 0)
  {
    if (s[ptr] == '@')
    {
      if (s[ptr + 1] == '@')
      {
        offset = demangle_insert_name(name, demangled, offset, length);
        demangle_params(s + ptr + 2, params, return_type, 0, length);
        return offset;
      }
        else
      {
        offset = demangle_name(s + ptr + 1, demangled, params, return_type, offset, length);
        if (offset < length) { demangled[offset++] = ':'; }
        if (offset < length) { demangled[offset++] = ':'; }
        offset = demangle_insert_name(name, demangled, offset, length);
        return offset;
      }
    }

    ptr++;
  }

  if (offset < length) { demangled[offset] = 0; }

  return offset;
}

int demangle(const char *name, char *demangled, int length)
{
  if (name[0] != '?')
  {
    snprintf(demangled, length, "%s", name);
    return -1;
  }

  char temp[length];
  char params[length];
  char return_type[length];

  temp[0] = 0;
  params[0] = 0;
  return_type[0] = 0;

  int offset = demangle_name(name + 1, temp, params, return_type, 0, length);
  demangled[length - 1] = 0;
  if (offset < length) { demangled[offset] = 0; }

  snprintf(demangled, length, "%s %s%s", return_type, temp, params);

  return offset;
}

