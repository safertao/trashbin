CC = gcc
CFLAGS = -fPIC --shared -W -std=c11 -pedantic

.PHONY: clean 

all: unlink
	
unlink: unlink.c makefile
	$(CC) $(CFLAGS) unlink.c -o unlink.so
	
clean:
	rm -f unlink.so