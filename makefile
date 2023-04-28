CC = gcc
CFLAGS = -fpic -shared -W -Werror -std=c11 -pedantic 

.PHONY: clean 

all: unlink
	
unlink: unlink.c
	$(CC) $(CFLAGS) unlink.c -o unlink.so
	
clean:
	rm -f unlink.so