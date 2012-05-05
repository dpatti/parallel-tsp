#include "aco.h"
// ant_t *ant, *ant2;

// int size = 100;
// ant = (ant_t*) malloc(1 * sizeof(ant_t) + size * sizeof(unsigned char));
// ant2 = (ant_t*) malloc(1 * sizeof(ant_t) + size * sizeof(unsigned char));
// printf("%d\n", sizeof(ant_t));
// int i;
// for (i = 0; i < size; i ++){
//   ant->visited[i] = i;
//   ant2->visited[i] = size-i;
// }
// for (i = 0; i < size; i ++){
//   printf("%d %d\n", ant->visited[i], ant2->visited[i]);
// }
// free(ant);

ant_t *ant_allocate(){
  return (ant_t*) malloc(         1 * sizeof(ant_t) +          // size of ant struct
                         graph_size * sizeof(nodeid_t));   // size of path
}

ant_t *ant_reset(ant_t *ant, int start) {
  ant->tour_length = 0;
  ant->first_node = start;
  ant->current_node = start;
  ant->visited_nodes = 0;
  memset(ant->path, -1, graph_size * sizeof(nodeid_t));
  return ant;
}

void ant_choose(ant_t *ant) {
  int i;
  float chance, total_chance=0;

  if (ant == NULL)
    return;
  // printf("ant_choose at %d, did %d\n", ant->current_node, ant->visited_nodes);

  // Check if ant is done
  if (ant->visited_nodes == graph_size)
    return ant_finish(ant);

  // Increment counter since an ant visited a node of ours
  completed_ants++;

  // Check if ant must return home
  if (ant->visited_nodes == graph_size - 1)
    return ant_send(ant, ant->first_node);

  // Ant is at one of our nodes, figure out where he is going next
  for (i = 0; i < graph_size; i++) {
    // Don't send ant to the node it is currently on
    // or if it visited already
    if (i == ant->current_node || ant->path[i] >= 0) {
      edge_chances[i] = 0;
      continue;
    }

    chance = pow(graph_edges[ant->current_node][i].pheromone, ALPHA) / pow(edge_hash(ant->current_node, i), BETA);
    // printf("%d: %.20f %f, %d\n", i, chance, graph_edges[ant->current_node][i].pheromone, edge_hash(ant->current_node, i));
    edge_chances[i] = chance;
    total_chance += chance;
  }

  chance = total_chance * rand() / RAND_MAX;
  // printf("chance: %.12f / %.12f\n", chance, total_chance);
  for (i = 0; i < graph_size; i++) {
    chance -= edge_chances[i];
    if (chance < 0) {
      // This is the edge
      ant_send(ant, i);
      return;
    }
  }
  // printf("Dead ant, that's bad\n");
  exit(1);
}

void ant_finish(ant_t *ant) {
  // printf("ant_finish\n");
  // exit(0);
  // push to new queue or something to keep ant safe
  queue_push(finished_queue, ant);
}

void ant_send(ant_t *ant, int next) {
  int dest_rank;

  ant->tour_length += edge_hash(ant->current_node, next);
  ant->path[ant->current_node] = next;
  ant->current_node = next;
  ant->visited_nodes++;

  dest_rank = get_rank(next);
  // printf("ant_send to %d on %d\n", next, dest_rank);
  if (dest_rank == mpi_rank)
    // XXX Maybe push to queue instead
    ant_choose(ant);
  else
    comm_send(ant, dest_rank);
}

void ant_retour(ant_t *ant) {
  int i;
  int local_nodes = graph_size / mpi_size + (mpi_rank < graph_size % mpi_size);
  for (i = 0; i < local_nodes; i++)
    graph_edges[i][get_node_id(ant->path[i])].pheromone += GLOBALDECAY / ant->tour_length;
}
