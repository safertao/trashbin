CC = gcc
CFLAGS = -fpic -shared -ggdb -W -Werror -std=c11 -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable

BUILD = ./build
SRC = ./src
unlink = $(BUILD)/unlink.so
unlinkc = $(SRC)/unlink.c
trash = $(BUILD)/trash
trashc = $(SRC)/trash.c

.PHONY: clean 

all: $(unlink) 
	
$(unlink): $(unlinkc)
	$(CC) $(CFLAGS) $(unlinkc) -o $(unlink)
	
clean:
	rm -f $(unlink) coursework.pdf