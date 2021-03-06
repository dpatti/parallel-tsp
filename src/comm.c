#include "aco.h"

typedef enum {NULL_REQUEST, RECV_REQUEST, SEND_REQUEST} req_t;

static int open_recvs=0;
static int open_sends=0;
static int buffer_size;
int next_index = 0;         // Rotating index to keep search time low

// While it may seem unnecessary to keep each of these in their own array,
// MPI_Request must be contiguous memory so that we can use MPI_Testsome()
static MPI_Request *req_mpi; // Each MPI request
static ant_t **req_ant;      // The ant refernce for the given request
static req_t *req_type;      // Whether the given request is a recv or send
static int *req_done;        // Return value from MPI_Testsome()

void comm_init(int ant_ct) {
  int i;

  assert(ant_ct > 0);

  buffer_size = ant_ct;
  req_mpi =  (MPI_Request *) malloc(buffer_size * sizeof(MPI_Request));
  req_ant =       (ant_t **) malloc(buffer_size * sizeof(ant_t *));
  req_type =       (req_t *) malloc(buffer_size * sizeof(req_t));
  req_done =         (int *) malloc(buffer_size * sizeof(int));

  for (i = 0; i < buffer_size; i++) {
    req_mpi[i] = MPI_REQUEST_NULL;
    req_ant[i] = NULL;
    req_type[i] = NULL_REQUEST;
  }
}

void comm_free() {
  free(req_mpi);
  free(req_ant);
  free(req_type);
}

void comm_close(int index, int *open_counter) {
  assert(index >= 0 && index < buffer_size);
  assert(open_counter != NULL);

  req_mpi[index] = MPI_REQUEST_NULL;
  req_ant[index] = NULL;
  req_type[index] = NULL_REQUEST;

  (*open_counter)--;
  assert(*open_counter >= 0);
}

// Find the next available index in our request array by using a rotating marker
// from last operation
int comm_next_index() {
  int i;
  // Find space based on our rotating index, use an i to help catch infinite
  // loops (i.e., buffer full, which should never happen)
  for (i = 0; i < buffer_size; i++) {
    if (req_type[next_index] == NULL_REQUEST)
      break;
    next_index = (next_index + 1) % buffer_size;
  }
  assert(i < buffer_size);

  return next_index;
}

void comm_send(ant_t *ant, int rank) {
  int index = comm_next_index();

  assert(ant != NULL);
  assert(rank >= 0 && rank < mpi_size);

  fprintf(debug, "[%d] open send to %d\n", mpi_rank, rank);
  MPI_Isend(ant, ANT_T_SIZE, MPI_BYTE, rank, 0, MPI_COMM_WORLD, &req_mpi[index]);
  req_ant[index] = ant;
  req_type[index] = SEND_REQUEST;
  open_sends++;
  send_count++;
}

void comm_recv(ant_t *ant) {
  int index = comm_next_index();

  assert(ant != NULL);

  MPI_Irecv(ant, ANT_T_SIZE, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req_mpi[index]);
  req_ant[index] = ant;
  req_type[index] = RECV_REQUEST;
  open_recvs++;
}

// Large comm methods ----------------------------------------------------------
// Main ACO loop
void comm_loop() {
  int ant_visits = 0;
  int i, count_done, index;

  // Loop until we know we aren't receing any more ants
  //  (i.e., every ant visited every core once, and the ants you started with an
  //         additional time)
  while ((ant_visits < (ant_count * local_nodes) + local_ants) || open_sends > 0) {
    fprintf(debug, "[%d] loop %d / %d (s: %d, r: %d)\n", mpi_rank, ant_visits, (ant_count * local_nodes) + local_ants, open_sends, open_recvs);
    // Check for received ants, and move any to process queue
    MPI_Testsome(buffer_size, req_mpi, &count_done, req_done, MPI_STATUS_IGNORE);
    fprintf(debug, "[%d] testsome done: %d\n", mpi_rank, count_done);
    for (i = 0; i < count_done; i++) {
      index = req_done[i];
      assert(req_type[index] != NULL_REQUEST);

      if (req_type[index] == SEND_REQUEST) {
        // Send requests are pushed to the spare as the memory is not needed
        fprintf(debug, "[%d] close send\n", mpi_rank);
        queue_push(spare_queue, req_ant[index]);
        comm_close(index, &open_sends);
      } else {
        // Recv requests are pushed to the process queue
        fprintf(debug, "[%d] close recv\n", mpi_rank);
        queue_push(process_queue, req_ant[index]);
        comm_close(index, &open_recvs);
      }
    }

    // Check if we need to post more receives
    while (open_recvs < MAX_RECVS && queue_size(spare_queue) > 0) {
      fprintf(debug, "[%d] open recv\n", mpi_rank);
      comm_recv(queue_pop(spare_queue));
    }

    // If the process queue is not empty, process one and loop
    if (queue_size(process_queue) > 0) {
      fprintf(debug, "[%d] process\n", mpi_rank);
      timer_start(compute_time);
      ant_choose(queue_pop(process_queue));
      timer_stop(compute_time);
      ant_visits++;
      // fprintf(debug, "Waiting for %d ants; %d done\n", (ant_count * local_nodes) + local_ants, ant_visits);
    } else {
      // New note: Waiting is bad, I think. Any request that causes waiting to
      // fall through will be marked as finished and not recognized in Testsome.

      // If it is empty, MPI_Wait() until something comes in and loop
      // fprintf(debug, "[%d] waitany %d+%d / %d\n", mpi_rank, open_sends, open_recvs, buffer_size);
      // MPI_Waitany(buffer_size, req_mpi, &count_done, MPI_STATUS_IGNORE);
      // fprintf(debug, "[%d] waitany-post\n", mpi_rank);
    }
  }

  // Before we end the ACO iteration, cancel any open requests
  assert(open_sends == 0);
  for (i = 0; i < buffer_size; i++) {
    if (req_type[i] == RECV_REQUEST) {
      MPI_Cancel(&req_mpi[i]);
      queue_push(spare_queue, req_ant[i]);
      comm_close(i, &open_recvs);
    }
  }
  assert(open_recvs == 0);
}

// Post-ACO trail sync
void comm_sync(int *tour_min, long *tour_sum) {
  int i, j, count=0, global_count, index;
  ant_t *ant;
  MPI_Request dummy;

  assert(*tour_min > 0 || queue_size(finished_queue) == 0);

  fprintf(debug, "[%d] comm_sync: %d\n", mpi_rank, *tour_min);

  // Reduce to find the total tour lengths taken
  MPI_Allreduce(MPI_IN_PLACE, tour_sum, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);

  // Reduce to find the lowest tour length of this iteration
  MPI_Allreduce(MPI_IN_PLACE, tour_min, 1, MPI_UNSIGNED, MPI_MIN, MPI_COMM_WORLD);
  fprintf(debug, "[%d] best ant: %d\n", mpi_rank, *tour_min);

  // Count ants on our core that match winning length
  for (i = 0; i < local_ants; i++) {
    ant = queue_pop(finished_queue);
    if (ant->tour_length == *tour_min) {
      count++;
      // Post this ant for sends. We're not using our comm array since
      //   a) We don't care about checking completion, we just know it will be
      //      when the barrier is complete at the end
      //   b) We are sending to every core, which can be a lot of entries in the
      //      array, which does not work for our model since there should always
      //      be enough room in the array for every unique ant allocation
      for (j = 0; j < mpi_size; j++) {
        if (j != mpi_rank) {
          MPI_Isend(ant, ANT_T_SIZE, MPI_BYTE, j, 0, MPI_COMM_WORLD, &dummy);
          send_count++;
        }
      }
      // Be ready to process these later when we are waiting for recvs
      queue_push(process_queue, ant);
      fprintf(debug, "[%d] posted ant send\n", mpi_rank);
    } else {
      // This ant is irrelevant and can be repurposed if needed
      queue_push(spare_queue, ant);
    }
  }

  // Reduce sum this count
  MPI_Allreduce(&count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  fprintf(debug, "[%d] reduced: %d %d\n", mpi_rank, count, global_count);

  // Post receives for (n - m) ants where n is the total count and m is how
  //   many you are sending out
  global_count -= count;
  // Receive as many as we can at once
  while (global_count > 0 && queue_size(spare_queue)) {
    ant = queue_pop(spare_queue);
    comm_recv(ant);
    global_count--;
    fprintf(debug, "[%d] open initial receive\n", mpi_rank);
  }

  // While we wait for that, process our own, if any
  while (queue_size(process_queue)) {
    ant = queue_pop(process_queue);
    timer_start(compute_time);
    ant_retour(ant);
    timer_stop(compute_time);
    queue_push(spare_queue, ant);
    fprintf(debug, "[%d] retoured ant\n", mpi_rank);
  }

  // Poll and process while receives are coming in
  while (open_recvs > 0) {
    MPI_Waitany(buffer_size, req_mpi, &index, MPI_STATUS_IGNORE);
    ant = req_ant[index];
    comm_close(index, &open_recvs);
    timer_start(compute_time);
    ant_retour(ant);
    timer_stop(compute_time);
    fprintf(debug, "[%d] received ant\n", mpi_rank);

    // Repost a recv if we have more
    if (global_count > 0) {
      comm_recv(ant);
      global_count--;
    } else {
      queue_push(spare_queue, ant);
    }
  }

  // We should be back to our initial state of only ants on spare
  fprintf(debug, "bs: %d  sq: %d  pq: %d  fq: %d  os: %d  or: %d\n",
      buffer_size,
      queue_size(spare_queue),
      queue_size(process_queue),
      queue_size(finished_queue),
      open_sends,
      open_recvs);
  assert( queue_size(spare_queue) == buffer_size &&
          queue_size(process_queue) == 0 &&
          queue_size(finished_queue) == 0 &&
          open_sends == 0 &&
          open_recvs == 0 );
}
