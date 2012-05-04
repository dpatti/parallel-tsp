#include "aco.h"

int main(int argc, char *argv[]) {
  int i, iterations=0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  // Parse command line arguments

  // Initialization of this core
  srand(time(NULL));
  edge_chances = (float *) malloc(graph_size * sizeof(float));

  // Allocate ants -- temp example
  ant_t *ant = ant_allocate();
  ant_reset(ant, 123);
  for (i = 0; i < graph_size; i++) {
    printf("%d\n", ant->path[i]);
  }

  // Start iterations
  for (i = 0; i < iterations; i++) {
    // Reset your state
    completed_ants = 0;


  }


  MPI_Finalize();
  return 0;
}

