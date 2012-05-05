#include "aco.h"

// ant_queue_t queues[num_queues];
ant_t *queues[num_queues][200];
int sizes[num_queues] = {0};

void queue_init(){
  memset(queues, 0, sizeof(queues));
}

int queue_size(queue_type type) {
  return sizes[type];
  // return queues[type].size;
}

void queue_push(queue_type type, ant_t *ant) {
  // printf("queue_push[%d] (%d)\n", type, sizes[type]+1);
  queues[type][sizes[type]] = ant;
  sizes[type]++;
}

ant_t *queue_pop(queue_type type) {
  // printf("queue_pop[%d] (%d)\n", type, sizes[type]-1);
  sizes[type]--;
  int i;
  ant_t *pop;
  pop = queues[type][0];
  for (i = 0; i < sizes[type]; i++)
    queues[type][i] = queues[type][i+1];
  return pop;
}

ant_t *queue_peek(queue_type type, int index) {
  // printf("queue_peek\n");
  return queues[type][index];
}
