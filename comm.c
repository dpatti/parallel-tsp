#include "aco.h"

void comm_next() {
  // Loop until we know we aren't receing any more ants
  //  (i.e., ants_seen = ants_total * local_cores)

  // Check for received ants, and move any to process queue
  // If the process queue is not empty, process one and loop
  // If it is empty, MPI_Wait() until something comes in and loop

  int local_nodes = graph_size / mpi_size + (mpi_rank < graph_size % mpi_size);
  while (completed_ants < ant_count * local_nodes) {
    // printf("Waiting for %d ants; %d done\n", ant_count * local_nodes, completed_ants);
    ant_choose(queue_pop(process_queue));
  }
}


void comm_send() {

}
