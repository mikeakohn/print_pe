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

static int demangle_insert_name(
  const char *name,
  char *demangled,
  int offset,
  int length)
{
  while (offset < length)
  {
    if (*name == 0 || *name == '@') { break; }
    demangled[offset++] = *name;
    name++;
  }

  if (offset < length) { demangled[offset] = 0; }

  return offset;
}

static int demangle_params(
  const char *name,
  char *demangled,
  int offset,
  int length)
{
  int count = 0;

  if (offset < length) { demangled[offset++] = '('; }

  if (name[0] == 'Y' && name[1] == 'A')
  {
    name += 2;

    while (offset < length)
    {
      if (*name == 0 || *name == '@') { break; }
      if (offset < length && count != 0) { demangled[offset++] = ','; }

      const char *type = NULL;

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
        case 'P':
        case 'Q':
        case 'V':
        case 'X':
        case 'Z': type = "...";           break;
        default: type = "???";            break;
      }

      if (type != NULL)
      {
        offset = demangle_insert_name(type, demangled, offset, length);
      }

      name++;
      count++;
    }
  }
    else
  {
    if (offset < length) { demangled[offset++] = '?'; }
  }

  if (offset < length) { demangled[offset++] = ')'; }
  if (offset < length) { demangled[offset] = 0; }

  return offset;
}

static int demangle_name(
  const char *name,
  char *demangled,
  char *params,
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
        demangle_params(s + ptr + 2, params, 0, length);
        return offset;
      }
        else
      {
        offset = demangle_name(s + ptr + 1, demangled, params, offset, length);
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

  char params[length];

  int offset = demangle_name(name + 1, demangled, params, 0, length);
  demangled[length - 1] = 0;
  if (offset < length) { demangled[offset] = 0; }

  if (strlen(demangled) + strlen(params) < length - 1);
  strcat(demangled, params);

  return offset;
}

