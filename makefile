CC=gcc
CFLAGS=-I.
DEPS = lexer.h

CXXFLAGS=-std=c++11

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: lexer.o main.o
	g++ -o compiler lexer.o main.o -I.
	
clean:
	rm -f *.o *.out

