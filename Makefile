
default:
	@+make -C build

clean:
	@rm -f build/*.o
	@rm -f *.bin
	@rm -f print_pe print_minidump print_pdb
	@rm -f print_pe.exe print_minidump.exe print_pdb
	@echo "Clean!"

cleanbin:
	rm -f *.bin

