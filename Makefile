CC = cc
CFLAGS = -Wall -g -O2  -Werror -std=gnu99 -Wno-unused-function 
LFLAGS = -ledit -lm

SRC_DIR = ./src

EXE = ./bin/main



PROGRAM = $(SRC_DIR)/main.c
PLIB = $(SRC_DIR)/lib/debug.c $(SRC_DIR)/lib/error.c $(SRC_DIR)/lib/mpc.c $(SRC_DIR)/expression/expr.c $(SRC_DIR)/lib/util.c

.PHONY:main
main:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(PLIB) $(PROGRAM) $(LFLAGS) -o $(EXE)
	./$(EXE)
gdb:
	gdb ./$(EXE)
run:
	./${EXE}
clean:
	rm -f *.o $(EXE)