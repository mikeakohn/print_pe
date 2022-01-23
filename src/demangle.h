/*

print_pe - Copyright 2005-2022 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef DEMANGLE_H
#define DEMANGLE_H

#include <stdint.h>

int demangle(const char *name, char *demangled, int length);

#endif

