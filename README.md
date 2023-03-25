
print_pe / print_minidump / print_pdb

Author: Michael Kohn
   Web: https://www.mikekohn.net/

Tools for dumping the contents of Window executables, MiniDump crash files,
and PDB files.

Features:
* Dump DOS header along with DOS binary.
* Dump all PE headers to readable text.
* Dump exported / imported function names.
* Extract resource files (.ico, .bmp, etc).
* Dump COM info including CLSID's.
* Limited ability to modify functions (make it return a value every time
  instead of running code.
* Dump information from Windows minidump (core) crash file.
* Dump information, including symbol addresses, from debug pdb files.
* Dump information from .lib files.

TODO:
* Dump .NET headers

How to use print_pe
-------------------

Find a DLL, EXE, OCX, or whatever Windows executable file and type:

    ./print_pe file.dll

To change a function called blah in file.dll to always return the value 100:

    ./print_pe -modify blah 100 file.dll

Highly recommended to make a backup of the DLL since it will overwrite
code that is already there.

How to use print_minidump
-------------------------

MiniDump files can be generated when a Windows program crashes or they
can be forced to be created on a running process. To dump the contents
of the file type:

    ./print_minidump filename.dmp

How to use print_pdb
--------------------

Since print_pdb has a lot of information, there are command line arguments
for filtering out which sections are displayed.

    Usage: print_pe [options] <input file>
      -all                     (Show all streams)
      -dir                     (Show directory stream)
      -tpi                     (Show TPI stream)
      -ipi                     (Show IPI stream)
      -symbols                 (Show symbols)
      -dump <filename> <index> (Dump stream at index to filename)

To show all the symbols in the pdb file, just type:

    ./print_pdb -symbols filename.pdb

To show all the streams in the pdb file, there is a -dir option and
the ability to dump that stream to a file so it could be analyzed with
another tool (maybe a hex editor) there is a -dump option. For example,
to dump the DBI debug information (which is always index 3):

    ./print_pdb -dump dbi.bin 3 filename.pdb

How to use print_lib
--------------------

    ./print_lib filename.lib

Notes
-----

It's possible that there is more data that can be extracted from these files.
If something is missing let me know and it can be added.

