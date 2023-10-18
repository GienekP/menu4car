CC = gcc
MAIN = menu4car

$(MAIN): $(MAIN).c $(MAIN).h
	$(CC) "$<" -o "$@"

$(MAIN).h : $(MAIN).bin
	xxd -i -c 16 "$<" "$@"

$(MAIN).bin : $(MAIN).asm
	mads "$<" -o:"$@"

