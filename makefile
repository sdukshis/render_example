CC = clang
CFLAGS = -g -Wall -pedantic
LFLAGS = -lm

.PHONY: all clean

all: render convert

convert: convert.o tga.o
	$(CC) $(LFLAGS) -o $@ $^

render: main.o tga.o draw.o model.o geometry.o
	$(CC) $(LFLAGS) -o $@ $^

main.o: main.c tga.h draw.h model.h geometry.h
	$(CC) -c $(CFLAGS) -o $@ $<

tga.o:tga.c tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

draw.o:draw.c draw.h tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

model.o:model.c model.h geometry.h
	$(CC) -c $(CFLAGS) -o $@ $<

geometry.o:geometry.c geometry.h
	$(CC) -c $(CFLAGS) -o $@ $<

convert.o:convert.c tga.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf render
	rm -rf *.o
