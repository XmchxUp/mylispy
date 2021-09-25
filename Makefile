CC = /usr/bin/gcc
CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function

SRC_DIR = ./src

EXE = ./bin/main

PROGRAM = $(SRC_DIR)/main.c
LIB = $(SRC_DIR)/lib/error.c

.PHONY:main
main:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(LIB) $(PROGRAM) -o $(EXE)
	./$(EXE)

clean:
	rm -f *.o $(EXE)