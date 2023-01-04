
filelist = main.c HostCommunication.c

main.exe: $(filelist)
	gcc -std=c99 $(filelist) -o $@

.PHONY: run
run: main.exe
	$<

