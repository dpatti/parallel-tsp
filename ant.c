#include "aco.h"

ant_t *ant_allocate(){
  return (ant_t*) malloc(ANT_T_SIZE);
}

ant_t *ant_reset(ant_t *ant, int start) {
  assert(ant != NULL);
  assert(start >= 0 && start < graph_size);

  ant->tour_length = 0;
  ant->first_node = start;
  ant->current_node = start;
  ant->visited_nodes = 0;
  memset(ant->path, -1, graph_size * sizeof(nodeid_t));
  return ant;
}

void ant_choose(ant_t *ant) {
  int i, last_edge=0;
  phero_t chance, total_chance=0;

  assert(ant != NULL);
  // printf("ant_choose at %d, did %d\n", ant->current_node, ant->visited_nodes);

  // Check if ant is done
  if (ant->visited_nodes == graph_size)
    return ant_finish(ant);

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

    last_edge = i;
    chance = pow(graph_edges[get_local_index(ant->current_node)][i].pheromone, ALPHA) / pow(edge_hash(get_local_index(ant->current_node), i), BETA);
    // printf("%d: %.20f %f, %d\n", i, chance, graph_edges[get_local_index(ant->current_node)][i].pheromone, edge_hash(ant->current_node, i));
    edge_chances[i] = chance;
    total_chance += chance;
    // printf("%02d %d : %.20f : %.20f\n", i, ant->visited_nodes, chance, total_chance);
  }

  int rng = rand();
  chance = total_chance * rng / ((float)RAND_MAX + 1);
  // printf("chance: %.12f / %.12f\n", chance, total_chance);
  for (i = 0; i < graph_size; i++) {
    chance -= edge_chances[i];
    if (chance < 0) {
      // This is the edge
      ant_send(ant, i);
      return;
    }
  }

  // If the chance is over 0, we had rounding errors and should use the last one
  if (chance > 0) {
    ant_send(ant, last_edge);
    return;
  }
  
  // All probabilities were zero in this case, so we had massive pheromone underflow
  printf("--------------------------------------------------------------------\n");
  printf("Dead ant, that's bad\n");
  printf("Random roll:  %d / %d\n", rng, RAND_MAX);
  printf("Total chance: %0.30f\n", total_chance);
  printf("Chance left:  %0.30f\n", chance);
  for (i = 0; i < graph_size; i++)
    printf("%c%02d %.12f : %.12f / %d\n", edge_chances[i]==0?'!':' ', i, edge_chances[i], graph_edges[get_local_index(ant->current_node)][i].pheromone, edge_hash(get_local_index(ant->current_node), i));
  exit(1);
}

void ant_finish(ant_t *ant) {
  assert(ant != NULL);
  queue_push(finished_queue, ant);
}

void ant_send(ant_t *ant, int next) {
  int dest_rank;

  assert(ant != NULL);
  assert(next >= 0 && next < graph_size);

  ant->tour_length += edge_hash(ant->current_node, next);
  ant->path[ant->current_node] = next;
  ant->current_node = next;
  ant->visited_nodes++;

  dest_rank = get_rank(next);
  // printf("ant_send to %d on %d\n", next, dest_rank);
  if (dest_rank == mpi_rank)
    queue_push(process_queue, ant);
  else
    comm_send(ant, dest_rank);
}

void ant_retour(ant_t *ant) {
  int i;
  assert(ant != NULL);
  for (i = 0; i < local_nodes; i++)
    graph_edges[i][ant->path[i]].pheromone += GLOBALDECAY / ant->tour_length;
}
