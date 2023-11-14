CC = gcc
MAIN = menu4car
LFLAGS=apultra/libapultra.a -Lapultra -lapultra $(ARCH)
CFLAGS=-c -O3 -g -fomit-frame-pointer -Iapultra/src/libdivsufsort/include -Iapultra/src $(ARCH)
SYSTEM = $(shell uname -s -m)

ifeq ($(SYSTEM)$(ARCH),Darwin arm64)
MNAME = $(shell uname -m)
$(MAIN): $(MAIN).x86_64 $(MAIN).arm64
	lipo -create -output $@ $^
	rm -f $^

$(MAIN).x86_64:
	$(MAKE) clean
	$(MAKE) ARCH="-target x86_64-apple-macos10.6"
	mv menu4car $@

$(MAIN).arm64:
	$(MAKE) clean
	$(MAKE) ARCH="-target arm64-apple-macos11"
	mv menu4car $@

else

$(MAIN): $(MAIN).o libapultra.a
	$(CC) "$<" $(LFLAGS) -o $@

endif

$(MAIN).o: $(MAIN).c $(MAIN).h
	echo $(ARCH)
	$(CC) "$<" $(CFLAGS) 

$(MAIN).h : $(MAIN).bin
	xxd -i -c 16 "$<" "$@"

test : $(MAIN).asm apldecr_zp.asm
	mads "$<" -t -o:menu4car.obx | sed "s/\\$$//g"

$(MAIN).bin : $(MAIN).asm apldecr_zp.asm
	mads "$<" -t -o:"$@" | sed "s/\\$$//g" | tee |  grep "#define" >menu4car_interface.h

libapultra.a:
	$(shell ./apultra_fix_makefile.sh)
	$(MAKE) -C apultra -f Makefile.menu4car libapultra.a

clean:
	rm -f menu4car
	rm -f *.o
	rm -f *.lab
	$(MAKE) -C apultra clean
