#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned char *visited;
  int destination;
} ant_t;

typedef struct {
  // unsigned weight; // might not need
  float pheromone;
} edge_t;

int rank, total;

unsigned elf_hash (void *key, int len);
unsigned edge_hash(int a, int b);

edge_t **allocate_graph(int local_nodes, int total_nodes);
void destroy_graph(edge_t **mem, int local_nodes);

int get_local_index(int node_id);
int get_node_id(int local_index);
