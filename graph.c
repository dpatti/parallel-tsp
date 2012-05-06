#include "aco.h"

// typedef char nodeid_t[3];
// 
// void set_index(nodeid_t *path, int index, int id) {
//   memcpy(path[index], (char*)&id+1, 3);
// }
// 
// int get_index(nodeid_t *path, int index) {
//   int a = 0;
//   memcpy((char*)&a+1, path[index], 3);
//   return a;
// }

edge_t **graph_allocate(int local_nodes, int total_nodes) {
  edge_t **mem;
  int i, j;

  mem = (edge_t**) malloc(local_nodes * sizeof(edge_t*));
  for (i = 0; i < local_nodes; i++) {
    mem[i] = (edge_t*) malloc(total_nodes * sizeof(edge_t));
    for (j = 0; j < total_nodes; j++) {
      mem[i][j].pheromone = INITIAL_PHEROMONE;
    }
  }

  return mem;
}

void graph_destroy(edge_t **mem, int local_nodes) {
  int i;
  for (i = 0; i < local_nodes; i++)
    free(mem[i]);
  free(mem);
}

// round robin distribution
// Local vertex identifier
int get_local_index(int node_id) {
  assert(node_id >= 0 && node_id < graph_size);
  return node_id / mpi_size;
}
// Global vertex identifier
int get_node_id(int local_index) {
  assert(local_index >= 0 && local_index < local_nodes);
  return mpi_rank + local_index * mpi_size;
}
// Global vertex identifier to MPI rank
int get_rank(int node_id) {
  assert(node_id >= 0 && node_id < graph_size);
  return node_id % mpi_size;
}
