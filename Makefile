CC = gcc
MAIN = menu4car

# optional apultra
PATH_TO_APULTRA=apultra/src/libapultra.h

ifneq ("$(wildcard $(PATH_TO_APULTRA))","")
LFLAGS_APULTRA=apultra/libapultra$(SUF).a -Lapultra -lapultra$(SUF) -DAPULTRA
O_APULTRA = libapultra$(SUF).a
CFLAGS_APULTRA = -Iapultra/src/libdivsufsort/include -Iapultra/src -DAPULTRA
else
LFLAGS_APULTRA = 
CFLAGS_APULTRA = 
endif

# obligatory ZX0 - easy as 3 files are used straight from zx0 repo.
LFLAGS_ZX0 = optimize$(SUF).o compress$(SUF).o memory$(SUF).o
O_ZX0 = optimize$(SUF).o compress$(SUF).o memory$(SUF).o

LFLAGS=$(LFLAGS_APULTRA) $(LFLAGS_ZX0) $(ARCH)
CFLAGS=-c -O3 -g -fomit-frame-pointer $(CFLAGS_APULTRA) $(ARCH)
SYSTEM = $(shell uname -s -m)

ifeq ($(SYSTEM)$(ARCH),Darwin arm64)
MNAME = $(shell uname -m)
$(MAIN): $(MAIN).x86_64 $(MAIN).arm64
	lipo -create -output $@ $^

$(MAIN).x86_64: $(MAIN).c $(MAIN).h
	$(MAKE) ARCH="-target x86_64-apple-macos10.6" SUF=.x86_64 

$(MAIN).arm64: $(MAIN).c $(MAIN).h
	$(MAKE) ARCH="-target arm64-apple-macos11" SUF=.arm64

else

$(MAIN)$(SUF): $(MAIN)$(SUF).o $(O_ZX0) $(O_APULTRA) 
	@echo "DOING " $@
	$(CC) "$<" $(LFLAGS) -o $@

endif

memory$(SUF).o: ZX0/src/memory.c
	$(CC) "$<" $(CFLAGS) -o $@
	
compress$(SUF).o: ZX0/src/compress.c
	$(CC) "$<" $(CFLAGS) -o $@
	
optimize$(SUF).o: ZX0/src/optimize.c
	$(CC) "$<" $(CFLAGS) -o $@
	
$(MAIN)$(SUF).o: $(MAIN).c $(MAIN).h $(MAIN).bin ramdata.h
	@echo "DOING " $@
	$(CC) "$<" $(CFLAGS) -o $@

ramdata.h :ramdata.bin
	xxd -i -c 16 "$<" "$@"

$(MAIN).h : $(MAIN).bin
	xxd -i -c 16 "$<" "$@"

test : $(MAIN).asm apldecr.asm zx0decr.asm
	mads "$<" -t -o:menu4car.obx | sed "s/\\$$//g"

ramdata.bin : ramdata.asm
	mads "$<" -t -o:$@ | sed "s/\\$$//g" | tee |  grep "#define" >menu4car_interface_ram.h


$(MAIN).bin : $(MAIN).asm apldecr.asm zx0decr.asm ramdata.bin
	mads "$<" -t -l -o:"$@" | sed "s/\\$$//g" | tee |  grep "#define" >menu4car_interface_flash.h

libapultra$(SUF).a:
	$(shell ./apultra_fix_makefile.sh)
	$(MAKE) -C apultra -f Makefile.menu4car libapultra$(SUF).a SUF=$(SUF)

clane: clean

claen: clean

clena: clean

celan: clean

clean:
	rm -f menu4car
	rm -f menu4car.exe
	rm -f *.o
	rm -f *.lab
	rm -f *.lst
	rm -f menu4car.bin
	rm -f menu4car.h
	rm -f ramdata.h
	rm -f ramdata.bin
	rm -f menu4car.arm64
	rm -f menu4car.x86_64
	rm -f menu4car_interface_*.h
	$(MAKE) -f Makefile.menu4car -C apultra clean SUF=.arm64
	$(MAKE) -f Makefile.menu4car -C apultra clean SUF=.x86_64
	$(MAKE) -f Makefile -C apultra clean

remake: clean
	$(MAKE) -j 12
	
