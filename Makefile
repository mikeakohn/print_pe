
default:
	@+make -C build

clean:
	@rm -f build/*.o
	@rm -f *.bin
	@rm -f print_pe print_pe.exe print_minidump print_minidump.exe
	@echo "Clean!"

cleanbin:
	rm -f *.bin

