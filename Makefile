CC = cc
CFLAGS = -Wall -g -O2  -Werror -std=gnu99 -Wno-unused-function 
LFLAGS = -ledit -lm

SRC_DIR = ./src

EXE = ./bin/main



PROGRAM = $(SRC_DIR)/main.c
PLIB = $(SRC_DIR)/lib/error.c $(SRC_DIR)/lib/mpc.c

.PHONY:main
main:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(PLIB) $(PROGRAM) $(LFLAGS) -o $(EXE)
	./$(EXE)
cgdb:
	cgdb ./$(EXE)
run:
	./${EXE}
clean:
	rm -f *.o $(EXE)