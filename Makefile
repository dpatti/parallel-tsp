NAME=aco
SRC=main.c parseargs.c hash.c graph.c ant.c comm.c queue.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -g -O3 -rdynamic
RM=rm -f
CC=mpicc

all: $(OBJ)
	mpicc $(CFLAGS) -o $(NAME) $(OBJ) -lm

run: all
	mpirun -np 1 ./aco

multicore: all
	mpirun -np 2 ./aco --ants 1 --iterations 1000

multiant: all
	mpirun -np 2 ./aco --ants 2

full: all
	mpirun -np 4 ./aco --iterations 1000

verbose: all
	mpirun -np 4 ./aco --iterations 1000 -v

kratos: all
	mpirun -np 4 ./aco --graph_size 1024 --iterations 1000 > kratos.out

clean:
	-$(RM) *.o

fclean: clean
	-$(RM) $(NAME)
