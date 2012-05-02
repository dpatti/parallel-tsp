NAME=aco
SRC=main.c hash.c graph.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -g
RM=rm -f

all: $(OBJ)
	cc ${CFLAGS} -o $(NAME) $(OBJ)

run: all
	./aco

clean:
	-$(RM) *.o

fclean: clean
	-$(RM) $(NAME)
