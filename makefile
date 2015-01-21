CC = clang
CFLAGS = -g -Wall -pedantic
LFLAGS = -lm

.PHONY: all clean

all: render

render: main.o tga.o
	$(CC) $(LFLAGS) -o $@ $^

main.o: main.c tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

tga.o:tga.c tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf render
	rm -rf *.o
