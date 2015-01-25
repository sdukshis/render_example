CC = clang
CFLAGS = -g -Wall -pedantic
LFLAGS = -lm

.PHONY: all clean

all: render

render: main.o tga.o draw.o model.o
	$(CC) $(LFLAGS) -o $@ $^

main.o: main.c tga.h draw.h model.h
	$(CC) -c $(CFLAGS) -o $@ $<

tga.o:tga.c tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

draw.o:draw.c draw.h tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

model.o:model.c model.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf render
	rm -rf *.o
