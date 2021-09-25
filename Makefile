CC = /usr/bin/gcc
CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function 
LFLAGS = -ledit

SRC_DIR = ./src

EXE = ./bin/main



PROGRAM = $(SRC_DIR)/main.c
PLIB = $(SRC_DIR)/lib/error.c

.PHONY:main
main:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(PLIB) $(PROGRAM) $(LFLAGS) -o $(EXE)
	./$(EXE)

clean:
	rm -f *.o $(EXE)