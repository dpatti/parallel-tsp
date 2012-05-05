#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#ifdef __blrts__
// Blue Gene
#define CLOCK_RATE 700000000.0
#else
// Kratos
#define CLOCK_RATE 2666700000.0
#endif
#define INITIAL_PHEROMONE 0.1   // Parameter: Initial pheromone trail value
#define ALPHA             1     // Parameter: Likelihood of ants to follow pheromone trails (larger value == more likely)
#define BETA              5     // Parameter: Likelihood of ants to choose closer nodes (larger value == more likely)
#define LOCALDECAY        0.2   // Parameter: Governs local trail decay rate [0, 1]
#define LOCALUPDATE       0.4   // Parameter: Amount of pheromone to reinforce local trail update by
#define GLOBALDECAY       0.2   // Parameter: Governs global trail decay rate [0, 1]

#define DEFAULT_GRAPH     16

#define ANT_T_SIZE        (sizeof(ant_t) + graph_size * sizeof(nodeid_t))
#define debug(...) printf(__VA_ARGS__)

typedef int nodeid_t;
typedef float phero_t;

typedef struct ant {
  int tour_length;
  int first_node;
  int current_node;
  int visited_nodes;
  struct ant *next;
  nodeid_t path[];
} ant_t;

typedef struct {
  // unsigned weight; // might not need
  phero_t pheromone;
} edge_t;

// Globals
int mpi_rank, mpi_size;
edge_t **graph_edges;
phero_t *edge_chances;
int completed_ants;
// Arguments
int graph_size;
int ant_count;
int iterations;

// Hash functions
unsigned elf_hash(void *key, int len);
unsigned edge_hash(int a, int b);

// Graph functions
edge_t **graph_allocate(int local_nodes, int total_nodes);
void graph_destroy(edge_t **mem, int local_nodes);

// Round Robin functions
int get_local_index(int node_id);
int get_node_id(int local_index);
int get_rank(int node_id);

// Ant functions
ant_t *ant_allocate();
ant_t *ant_reset(ant_t *ant, int start);
void ant_choose(ant_t *ant);
void ant_finish();
void ant_send(ant_t *ant, int next);
void ant_retour(ant_t *ant);

// MPI Communication
void comm_next();
void comm_send();

// Queue implementation
typedef struct {
  ant_t *HEAD;
  ant_t *TAIL;
  int size;
} queue_t;
typedef enum {spare_queue, process_queue, receive_queue, send_queue, finished_queue, num_queues} queue_type;
void queue_init();
int queue_size(queue_type type);
void queue_push(queue_type type, ant_t *ant);
ant_t *queue_pop(queue_type type);
ant_t *queue_peek(queue_type type);
