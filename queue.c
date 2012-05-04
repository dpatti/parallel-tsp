#include "aco.h"

ant_queue_t queues[num_queues];

void queue_init(){
  memset(queues, 0, sizeof(queues));
}

int queue_size(queue_type type) {
  return 0;
  // return queues[type].size;
}

void queue_push(queue_type type, ant_t *ant) {

}

ant_t *queue_pop(queue_type type) {
  return NULL;
}
