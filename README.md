
print_pe

Author: Michael Kohn
   Web: http://www.mikekohn.net/

I started this program back in 2005 under a less "professional" sounding
name, so I decided to rename it and put it on github.  I did a little bit
of code clean-up on it as well.. still needs some more really.

I recently started working on adding code to dump .NET info so I could add
.NET support to Java Grinder (http://www.mikekohn.net/micro/java_grinder.php)
but I haven't had time to finish it.

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

Highly recommended you make a backup of the DLL since it will overwrite
code that is already there.






