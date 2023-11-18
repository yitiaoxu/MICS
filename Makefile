
c_args  = -D__HC_DEBUG
h_files = ansicolorconsole.h asciiControlCode.h \
          HostCommunication.h HostCommunicationCommand.h HostCommunicationCommandCode.h HostCommunicationErrorCode.h \
          HostCommunicationUtils.h PublicCache.h
c_files = main.c HostCommunication.c HostCommunicationHooks.c HostCommunicationUtils.c PublicCache.c

.PHONY: default
default: main.exe dump

main.exe: $(h_files) $(c_files)
	gcc -std=c99 $(c_args) $(c_files) -o $@

.PHONY: dump
dump: main.exe
	objdump -x main.exe > main.dump.hh
	objdump -D main.exe > main.dump
#	objdump -S main.exe > main.S


.PHONY: run
run: main.exe
	./$<
