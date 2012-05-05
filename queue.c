#include "aco.h"

#define debug(...) 

queue_t queues[num_queues];
int sizes[num_queues] = {0};

void queue_init(){
  memset(queues, 0, sizeof(queues));
}

int queue_size(queue_type type) {
  return sizes[type];
  // return queues[type].size;
}

void queue_push(queue_type type, ant_t *ant) {
  debug("queue_push[%d] (%d)\n", type, sizes[type]+1);
  ant->next = NULL;
  if (queues[type].HEAD == NULL)
    queues[type].HEAD = ant;
  if (queues[type].TAIL)
    queues[type].TAIL->next = ant;
  queues[type].TAIL = ant;
  sizes[type]++;
}

ant_t *queue_pop(queue_type type) {
  debug("queue_pop[%d] (%d)\n", type, sizes[type]-1);
  ant_t *pop = queues[type].HEAD;
  queues[type].HEAD = queues[type].HEAD->next;
  if (queues[type].TAIL == pop)
    queues[type].TAIL = NULL;
  sizes[type]--;
  assert(sizes[type] >= 0);

  return pop;
}

ant_t *queue_peek(queue_type type) {
  debug("queue_peek\n");
  return queues[type].HEAD;
}
