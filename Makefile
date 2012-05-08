NAME=aco
SRC=src/main.c src/parseargs.c src/hash.c src/graph.c src/ant.c src/comm.c src/queue.c src/timers.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -g -O3 -rdynamic
RM=rm -f
CC=mpicc

all: $(OBJ)
	mpicc $(CFLAGS) -o $(NAME) $(OBJ) -lm

# Builds project on BlueGene
bluegene:
	mpixlc -O5 -qarch=440d -qtune=440 -o ${NAME} ${SRC}

# Queues the batch script on the BlueGene
queue:
	mkdir -p out
	sbatch -p bigmem --nodes 512 -t 10 -o ./out/lastrun ./scripts/blue_gene_run.sh 128

# Checks results of the latest test (can be used while running)
results:
	find out -newer out/lastrun -name "std*" | xargs tail -n9

run: all
	mpirun -np 1 ./aco
multicore: all
	mpirun -np 2 ./aco --ants 1
multiant: all
	mpirun -np 2 ./aco --ants 2
full: all
	mpirun -np 4 ./aco --iterations 1000
verbose: all
	mpirun -np 4 ./aco --iterations 1000 -v
kratos: all
	mpirun -np 8 ./aco --graph_size 1024 --iterations 1000 > kratos.out

pdf:
	make tex/make pdf
	pdflatex tex/main.tex

parse:
	-$(RM) out/results.dat
	find out/kratos -type f -exec ruby scripts/parse.rb {} \; | sort >> out/parsed.dat
	find out/bluegene -type f -exec ruby scripts/parse.rb {} \; | sort >> out/parsed.dat

compile:
	mkdir -p graph/png
	ruby scripts/compile.rb out/results.dat graph/
	gnuplot < scripts/plot.p

clean:
	-$(RM) src/*.o
	-$(RM) tex/*.aux
	-$(RM) tex/*.log
	-$(RM) tex/*.pdf

fclean: clean
	-$(RM) $(NAME)
