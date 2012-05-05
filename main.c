#include "aco.h"

void parseargs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  int i, iter;
  int local_nodes, local_ants;
  int best_tour, best_iter=0, best_ct=0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  // Parse command line arguments
  graph_size = DEFAULT_GRAPH;
  ant_count = 1; // DEFAULT_GRAPH;
	iterations = 20;
	parseargs(argc, argv);


  // Initialization of this core
  srand(time(NULL));
  local_nodes = graph_size / mpi_size + (mpi_rank < graph_size % mpi_size);
  local_ants = ant_count / mpi_size + (mpi_rank < ant_count % mpi_size);
  graph_edges = graph_allocate(local_nodes, graph_size);
  edge_chances = (float *) malloc(graph_size * sizeof(float));
  queue_init();

  // Allocate ants -- temp example
  for (i = 0; i < ant_count; i++)
    queue_push(spare_queue, ant_allocate());

  // Start iterations
  for (iter = 0; iter < iterations; iter++) {
    // printf("Iteration %d\n", iter);
    // Reset your state
    completed_ants = 0;
    // Setup initial ant queue
    while (queue_size(finished_queue))
      queue_push(spare_queue, queue_pop(finished_queue));
    for (i = 0; i < local_ants; i++)
      // XXX: the i below only works for one core
      queue_push(process_queue, ant_reset(queue_pop(spare_queue), i));

    // Do entire ACO algorithm
    // printf("Starting ACO\n");
    comm_next();
    // printf("Ending ACO\n");

    // All ranks finished
    // Find ant(s) with lowest tour length
    int tour = -1;
    int temp;
    for (i = 0; i < ant_count; i++) {
      temp = queue_peek(finished_queue, i)->tour_length;
      if (tour == -1 || temp < tour) {
        tour = temp;
      }
    }
    // Reduce to find the lowest tour length of this iteration
    // Count ants on our core that match winning length
    // Reduce sum this count
    // Post receives for (n - m) ants where n is the total count and m is how
    //   many you are sending out 
    // After all receives done, do pheromone reduction of each local edge
    int j;
    for (i = 0; i < local_nodes; i++) {
      for (j = 0; j < graph_size; j++) {
        graph_edges[i][j].pheromone *= (1 - GLOBALDECAY);
      }
    }
    // Then, iterate over each ant
    //   Add to pheromone levels
    for (i = 0; i < ant_count; i++)
      if (queue_peek(finished_queue, i)->tour_length == tour)
        ant_retour(queue_peek(finished_queue, i));
    printf("Best ant for iteration %d: %d\n", iter, tour);
    if (!best_iter || tour < best_tour) {
      best_tour = tour;
      best_iter = iter;
      best_ct = 1;
    } else if (tour == best_tour) {
      best_ct++;
    }
  }

  printf("Best solution was %d found %d times; first on tour %d\n", best_tour, best_ct, best_iter);

  // Cleanup
  for (i = 0; i < num_queues; i++)
    while (queue_size(i))
      free(queue_pop(i));
  graph_destroy(graph_edges, local_nodes);

  MPI_Finalize();
  return 0;
}

