#include <stdio.h>
#include <stdlib.h>

#include "demangle.h"

int main(int argc, char *argv[])
{
  //const char *name = "?AddNums@@YAHHH@Z";

  if (argc != 2)
  {
    printf("Usage: %s <name>\n", argv[0]);
    exit(0);
  }

  char demangled[1024];
  demangle(argv[1], demangled, sizeof(demangled));

  printf("%s\n", argv[1]);
  printf("%s\n", demangled);

  return 0;
}

