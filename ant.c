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
  ant->current_node = start;
  ant->visited_nodes = 0;
  memset(ant->path, -1, graph_size * sizeof(nodeid_t));
  return ant;
}

void ant_choose(ant_t *ant) {
  int i;
  float chance, total_chance;

  if (ant == NULL)
    return;

  // Check if ant is done
  if (ant->visited_nodes == graph_size)
    ant_finish(ant);

  // Ant is at one of our nodes, figure out where he is going next
  for (i = 0; i < graph_size; i++) {
    // Don't send ant to the node it is currently on
    // or if it visited already
    if (i == ant->current_node || ant->path[i] >= 0)
      continue;

#define pheromone 1
    chance = pow(pheromone, ALPHA) / pow(edge_hash(ant->current_node, i), BETA);
    edge_chances[i] = chance;
    total_chance += chance;
  }

  chance = total_chance * rand() / RAND_MAX;
  for (i = 0; i < graph_size; i++) {
    chance -= edge_chances[i];
    if (chance < 0) {
      // This is the edge
      ant_send(ant, i);
    }
  }
}

void ant_finish() {
  completed_ants++;
  // maybe store ant
}

void ant_send(ant_t *ant, int next) {
  int dest_rank;

  ant->path[ant->current_node] = next;
  ant->current_node = next;
  ant->visited_nodes++;

  dest_rank = get_rank(next);
  if (dest_rank == mpi_rank)
    ant_choose(ant);
  else
    comm_send(ant, dest_rank);
}
