#include "aco.h"

// Elf hash implementation for round robin (pseudo-randomish)
int hash_round_robin(int a, int b) {
  int key[] = {a, b};
  unsigned char *p = (unsigned char*)key;
  unsigned h = 0, g;
  int i;

  for (i = 0; i < sizeof(int)*2; i++) {
    h = (h << 4) + p[i];
    g = h & 0xf0000000L;

    if (g != 0)
      h ^= g >> 24;

    h &= ~g;
  }

  return h % 256;
}

// Difference of squared indicies, square rooted
int hash_distance(int a, int b) {
  return (int) sqrt(b * b - a * a);
}

// Same as distance with emphasis on staying within cluster
int hash_clustering(int a, int b) {
  return (int) sqrt(b * b - a * a) + (get_rank(a) - get_rank(b));
}

unsigned edge_hash(int a, int b) {
  int swap;

  assert(a >= 0 && b >= 0);

  if (b < a) {
    swap = a;
    a = b;
    b = swap;
  }

  return hash_method[method](a, b) + 1;
}
