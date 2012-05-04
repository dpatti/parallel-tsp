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
  sizes[type]++;
  queues[type][sizes[type]] = ant;
}

ant_t *queue_pop(queue_type type) {
  sizes[type]--;
  int i;
  ant_t *pop;
  pop = queues[type][0];
  for (i = 0; i < sizes[type]; i++)
    queues[type][i] = queues[type][i+1];
  return pop;
}
