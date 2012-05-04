NAME=aco
SRC=main.c hash.c graph.c ant.c comm.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -g
RM=rm -f
CC=mpicc

all: $(OBJ)
	mpicc $(CFLAGS) -o $(NAME) $(OBJ) -lm

run: all
	mpirun -np 1 ./aco

clean:
	-$(RM) *.o

fclean: clean
	-$(RM) $(NAME)
