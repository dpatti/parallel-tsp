#include "aco.h"

void parseargs(int argc, char *argv[]);

// static void dump_trace() {
// 	void * buffer[255];
// 	const int calls = backtrace(buffer, sizeof(buffer) / sizeof(void *));
// 	backtrace_symbols_fd(buffer, calls, 1);
// 	exit(EXIT_FAILURE);
// }

int main(int argc, char *argv[]) {
  int i, j, iter, tour_min, ant_buffer_size;
  long tour_sum;
  int best_tour=0, best_iter=0, best_ct=0;
  ant_t *ant_iter;

  // signal(SIGSEGV, dump_trace);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  hash_method[round_robin] = hash_round_robin;
  hash_method[distance]    = hash_distance;
  hash_method[clustering]  = hash_clustering;

  // Parse command line arguments
  graph_size = DEFAULT_GRAPH;
  ant_count = DEFAULT_GRAPH;
	iterations = 20;
  method = round_robin;
	parseargs(argc, argv);
  graph_size = (graph_size + mpi_size - 1) / mpi_size * mpi_size;

  if (verbose)
    debug = stdout;
  else
    debug = fopen("/dev/null", "w");
  // Shit is bugged
  // // Set up a new file descriptor(3) and handle(debug) for debug
  // dup2(1, 3);
  // debug = fdopen(3, "a");
  // // By closing it now, we can still have valid fprintfs, but it won't go anywhere
  // if (!verbose)
  //   close(3);

  // Initialization of this core
  srand(time(NULL));

  local_nodes = graph_size / mpi_size + (mpi_rank < graph_size % mpi_size);
  local_ants = ant_count / mpi_size + (mpi_rank < ant_count % mpi_size);
  ant_buffer_size = (ant_count / mpi_size + 1) * BUFFER_RATIO;

  graph_edges = graph_allocate(local_nodes, graph_size);
  edge_chances = (phero_t *) malloc(graph_size * sizeof(phero_t));
  queue_init();
  comm_init(ant_buffer_size);

  // Allocate ants -- temp example
  for (i = 0; i < ant_buffer_size; i++)
    queue_push(spare_queue, ant_allocate());

  // Start iterations
  timer_start(total_time);
  for (iter = 0; iter < iterations; iter++) {
    // Test pheromone printing
    // for (i = 0; i < local_nodes; i++) {
    //   printf("[%d] ", mpi_rank);
    //   for (j = 0; j < graph_size; j++) {
    //     printf("%.6f ", graph_edges[i][j].pheromone);
    //   }
    //   printf("\n");
    // }

    // fprintf(debug, "Iteration %d\n", iter);
    // Reset initial ant queue
    while (queue_size(finished_queue))
      queue_push(spare_queue, queue_pop(finished_queue));
    for (i = 0; i < local_ants; i++)
      queue_push(process_queue,
          ant_reset(
            queue_pop(spare_queue),
            get_node_id(i % local_nodes)));

    // Do entire ACO algorithm
    fprintf(debug, "Starting ACO\n");
    timer_start(aco_time);
    comm_loop();
    timer_stop(aco_time);
    // fprintf(debug, "Ending ACO\n");

    // Do pheromone reduction of each local edge
    for (i = 0; i < local_nodes; i++) {
      for (j = 0; j < graph_size; j++) {
        graph_edges[i][j].pheromone *= (1 - GLOBALDECAY);
        // fprintf(debug, "[%d][%d] = %.12f\n", i, j, graph_edges[i][j].pheromone);
      }
    }

    // Find ant(s) with lowest tour length
    tour_min = -1;
    tour_sum = 0;
    ant_iter = queue_peek(finished_queue);
    while (ant_iter != NULL) {
      if (tour_min == -1 || ant_iter->tour_length < tour_min)
        tour_min = ant_iter->tour_length;
      tour_sum += ant_iter->tour_length;
      ant_iter = ant_iter->next;
    }

    timer_start(sync_time);
    comm_sync(&tour_min, &tour_sum);
    timer_stop(sync_time);

    // Wait for the others
    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_rank == 0) printf("Iteration %4d | Best tour: %d, average tour: %.2lf\n", iter, tour_min, (double)tour_sum/ant_count);
    if (!best_iter || tour_min < best_tour) {
      best_tour = tour_min;
      best_iter = iter;
      best_ct = 1;
    } else if (tour_min == best_tour) {
      best_ct++;
    }
  }
  timer_stop(total_time);

  if (mpi_rank == 0){
    printf("Best solution was %d found %d times; first on tour %d\n", best_tour, best_ct, best_iter);
    printf("Ant size:       %u\n", ANT_T_SIZE);
    printf("Ant transfers:  %d\n", send_count);
    printf("Total time:     %.3lfs\n", timer_get(total_time));
    printf("  Compute time:   %.3lfs\n", timer_get(compute_time));
    printf("  Transfer time:  %.3lfs\n", timer_get(total_time) - timer_get(compute_time));
    printf("  ACO time:       %.3lfs\n", timer_get(aco_time));
    printf("  Sync time:      %.3lfs\n", timer_get(sync_time));
  }

  // Cleanup
  for (i = 0; i < num_queues; i++)
    while (queue_size(i))
      free(queue_pop(i));
  graph_destroy(graph_edges, local_nodes);
  comm_free();

  MPI_Finalize();
  return 0;
}

