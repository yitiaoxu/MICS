
filelist = main.c HostCommunication.c HostCommunicationHooks.c HostCommunicationUtils.c

.PHONY: default
default: main.exe dump

main.exe: $(filelist)
	gcc -std=c99 $(filelist) -o $@

.PHONY: dump
dump: main.exe
	objdump -x main.exe > main.dump.hh
	objdump -D main.exe > main.dump
#	objdump -S main.exe > main.S


.PHONY: run
run: main.exe
	$<
