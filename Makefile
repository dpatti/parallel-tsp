NAME=aco
SRC=main.c parseargs.c hash.c graph.c ant.c comm.c queue.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -g -O3
RM=rm -f
CC=mpicc

all: $(OBJ)
	mpicc $(CFLAGS) -o $(NAME) $(OBJ) -lm

run: all
	mpirun -np 1 ./aco

clean:
	-$(RM) *.o tex/*.aux tex/*.log tex/*.pdf

fclean: clean
	-$(RM) $(NAME)

pdf:
	make tex/make pdf
	pdflatex tex/main.tex
