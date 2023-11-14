CC = gcc
MAIN = menu4car
LFLAGS=apultra/libapultra$(SUF).a -Lapultra -lapultra$(SUF) $(ARCH)
CFLAGS=-c -O3 -g -fomit-frame-pointer -Iapultra/src/libdivsufsort/include -Iapultra/src $(ARCH)
SYSTEM = $(shell uname -s -m)

ifeq ($(SYSTEM)$(ARCH),Darwin arm64)
MNAME = $(shell uname -m)
$(MAIN): $(MAIN).x86_64 $(MAIN).arm64
	lipo -create -output $@ $^
	#rm -f $^

$(MAIN).x86_64:
	#$(MAKE) clean SUF=.x86_64
	$(MAKE) ARCH="-target x86_64-apple-macos10.6" SUF=.x86_64 
	#mv menu4car $@

$(MAIN).arm64:
	#$(MAKE) clean SUF=.arm64
	$(MAKE) ARCH="-target arm64-apple-macos11" SUF=.arm64
	#mv menu4car $@

else

$(MAIN)$(SUF): $(MAIN)$(SUF).o libapultra$(SUF).a
	@echo "DOING " $@
	$(CC) "$<" $(LFLAGS) -o $@

endif

$(MAIN)$(SUF).o: $(MAIN).c $(MAIN).h
	@echo "DOING " $@
	$(CC) "$<" $(CFLAGS) -o $@

$(MAIN).h : $(MAIN).bin
	xxd -i -c 16 "$<" "$@"

test : $(MAIN).asm apldecr_zp.asm
	mads "$<" -t -o:menu4car.obx | sed "s/\\$$//g"

$(MAIN).bin : $(MAIN).asm apldecr_zp.asm
	mads "$<" -t -o:"$@" | sed "s/\\$$//g" | tee |  grep "#define" >menu4car_interface.h

libapultra$(SUF).a:
	$(shell ./apultra_fix_makefile.sh)
	$(MAKE) -C apultra -f Makefile.menu4car libapultra$(SUF).a SUF=$(SUF)

clean:
	rm -f menu4car
	rm -f *.o
	rm -f *.lab
	rm -f menu4car.arm64
	rm -f menu4car.x86_64
	$(MAKE) -f Makefile.menu4car -C apultra clean SUF=.arm64
	$(MAKE) -f Makefile.menu4car -C apultra clean SUF=.x86_64

	
