CC = gcc
CFLAGS = -fpic -shared -ggdb -W -Werror -std=c11 -pedantic 

DIR = ./build
SRC = ./src
prog = $(DIR)/unlink.so
code = $(SRC)/unlink.c

.PHONY: clean 

all: $(prog)
	
$(prog): $(code)
	$(CC) $(CFLAGS) $(code) -o $(prog)
	
clean:
	rm -f $(prog)