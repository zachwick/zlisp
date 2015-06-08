.PHONY: clean build

build: zlisp

clean:
	rm -f zlisp zlisp.o

zlisp: zlisp.o
	gcc -std=gnu99 -Wall -ledit -lm -I. $< -o $@ 

zlisp.o: zlisp.c
	gcc -c $< -std=gnu99 -Wall -ledit -lm -I. -o $@ 
