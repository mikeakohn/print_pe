
default:
	@+make -C build

clean:
	@rm -f build/*.o
	@rm -f *.bin
	@rm -f print_pe print_minidump print_pdb test_demangle
	@rm -f print_pe.exe print_minidump.exe print_pdb
	@echo "Clean!"

.PHONY: test_demangle
test_demangle:
	gcc -o test_demangle src/test_demangle.c src/demangle.c -Wall

cleanbin:
	rm -f *.bin

