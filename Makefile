CC = /usr/bin/gcc
CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function 
LIBS = -ledit

SRC_DIR = ./src

EXE = ./bin/main



PROGRAM = $(SRC_DIR)/main.c
PLIB = $(SRC_DIR)/lib/error.c

.PHONY:main
main:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(PLIB) $(PROGRAM) $(LIBS) -o $(EXE)
	./$(EXE)

clean:
	rm -f *.o $(EXE)