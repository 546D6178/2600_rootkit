all: companion
companion : companion.c companion.h libRawTCP_Lib.a
	gcc -Wall -o $@ $^

clean:
	-rm *.o 	 
