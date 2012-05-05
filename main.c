#include "aco.h"

void parseargs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  int i, j, iter;
  int local_nodes, local_ants; // XXX: Best way to pass these? global?
  int best_tour=0, best_iter=0, best_ct=0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  mpi_rank = 0; mpi_size = 1;

  // Parse command line arguments
  graph_size = DEFAULT_GRAPH;
  ant_count = 100; // DEFAULT_GRAPH;
	iterations = 20;
	parseargs(argc, argv);

  // Initialization of this core
  srand(time(NULL));
  local_nodes = graph_size / mpi_size + (mpi_rank < graph_size % mpi_size);
  local_ants = ant_count / mpi_size + (mpi_rank < ant_count % mpi_size);
  graph_edges = graph_allocate(local_nodes, graph_size);
  edge_chances = (phero_t *) malloc(graph_size * sizeof(phero_t));
  queue_init();

  // Allocate ants -- temp example
  for (i = 0; i < ant_count; i++)
    queue_push(spare_queue, ant_allocate());

  // Start iterations
  for (iter = 0; iter < iterations; iter++) {
    // debug("Iteration %d\n", iter);
    // Reset your state
    completed_ants = 0;
    // Setup initial ant queue
    while (queue_size(finished_queue))
      queue_push(spare_queue, queue_pop(finished_queue));
    for (i = 0; i < local_ants; i++)
      queue_push(process_queue,
          ant_reset(
            queue_pop(spare_queue),
            get_node_id(i % local_nodes)));

    // Do entire ACO algorithm
    // debug("Starting ACO\n");
    comm_next();
    // debug("Ending ACO\n");

    // All ranks finished
    // Do pheromone reduction of each local edge
    for (i = 0; i < local_nodes; i++) {
      for (j = 0; j < graph_size; j++) {
        graph_edges[i][j].pheromone *= (1 - GLOBALDECAY);
        // debug("[%d][%d] = %.12f\n", i, j, graph_edges[i][j].pheromone);
      }
    }
    // Find ant(s) with lowest tour length
    int tour = -1;
    ant_t *ant_iter = queue_peek(finished_queue);
    while (ant_iter != NULL) {
      if (tour == -1 || ant_iter->tour_length < tour)
        tour = ant_iter->tour_length;
      ant_iter = ant_iter->next;
    }
    assert(tour > 0);

    // Reduce to find the lowest tour length of this iteration
    MPI_Allreduce(&tour, MPI_IN_PLACE, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    // Count ants on our core that match winning length
    int count = 0, global_count;
    ant_t *buffer;
    for (i = 0; i < local_ants; i++) {
      buffer = queue_pop(finished_queue);
      if (buffer->tour_length == tour) {
        queue_push(send_queue, buffer);
        count++;
      } else {
        queue_push(finished_queue, buffer);
      }
    }
    // Reduce sum this count
    MPI_Allreduce(&count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    global_count -= count;

    // Post receives for (n - m) ants where n is the total count and m is how
    //   many you are sending out 
    MPI_Request *reqs = (MPI_Request*) malloc(global_count * sizeof(MPI_Request));
    MPI_Request empty;
    for (i = 0; i < global_count; i++) {
      buffer = queue_pop(spare_queue);
      MPI_Irecv(buffer, ANT_T_SIZE, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[i]);
      queue_push(receive_queue, buffer);
    }

    // Send your best ants, if any to all cores
    ant_iter = queue_peek(finished_queue);
    while (ant_iter != NULL) {
      if (ant_iter->tour_length == tour)
        count++;
      ant_iter = ant_iter->next;
    }
    for (i = 0; i < count; i++) {
      buffer = queue_pop(send_queue);
      for (j = 0; j < mpi_size; j++) {
        MPI_Isend(buffer, ANT_T_SIZE, MPI_BYTE, j, 0, MPI_COMM_WORLD, &empty);
      }
      // We need to process these later
      queue_push(receive_queue, buffer);
    }

    // After all receives done, iterate over each ant
    for (i = 0; i < global_count; i++) {
      MPI_Wait(&reqs[i], MPI_STATUS_IGNORE);
      buffer = queue_pop(receive_queue);
      ant_retour(buffer);
      queue_push(spare_queue, buffer);
    }

    // Process the rest on the queue (ours)
    while (queue_size(receive_queue)) {
      buffer = queue_pop(receive_queue);
      ant_retour(buffer);
      queue_push(spare_queue, buffer);
    }

    // Wait for the others
    MPI_Barrier(MPI_COMM_WORLD);

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

