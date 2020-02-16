
print_pe

Author: Michael Kohn
   Web: http://www.mikekohn.net/

Tools for dumping the contents of Window executables and MiniDumps.

Features:
* Dump DOS header along with DOS binary.
* Dump all PE headers to readable text.
* Dump exported / imported function names.
* Extract resource files (.ico, .bmp, etc).
* Dump COM info including CLSID's.
* Limited ability to modify functions (make it return a value every time
  instead of running code.

TODO:
* Dump .NET headers

How To Use:

Find a DLL, EXE, OCX, or whatever Windows executable file and type:

./print_pe file.dll

To change a function called blah in file.dll to always return the value 100:

./print_pe -modify blah 100 file.dll

Highly recommended to make a backup of the DLL since it will overwrite
code that is already there.

