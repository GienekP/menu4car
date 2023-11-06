CC = gcc
MAIN = menu4car
LFLAGS=apultra/libapultra.a -Lapultra -lapultra
CFLAGS=-c -O3 -g -fomit-frame-pointer -Iapultra/src/libdivsufsort/include -Iapultra/src


$(MAIN): $(MAIN).o libapultra.a
	$(CC) "$<" $(LFLAGS) -o $@


$(MAIN).o: $(MAIN).c $(MAIN).h
	$(CC) "$<" $(CFLAGS) 

$(MAIN).h : $(MAIN).bin
	xxd -i -c 16 "$<" "$@"

$(MAIN).bin : $(MAIN).asm apldecr_zp.asm
	mads "$<" -t -o:"$@" | sed "s/\\$$//g"

libapultra.a:
	$(MAKE) -C apultra libapultra.a

clean:
	rm menu4car
	rm *.o
	rm *.lab
	$(MAKE) -C apultra clean
