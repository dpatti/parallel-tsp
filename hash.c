#include "aco.h"

unsigned elf_hash (void *key, int len) {
  unsigned char *p = (unsigned char*)key;
  unsigned h = 0, g;
  int i;

  for (i = 0; i < len; i++) {
    h = (h << 4) + p[i];
    g = h & 0xf0000000L;

    if (g != 0)
      h ^= g >> 24;

    h &= ~g;
  }

  return h;
}

unsigned edge_hash(int a, int b) {
  int key[2];
  if (a < b) {
    key[0] = a;
    key[1] = b;
  } else {
    key[0] = b;
    key[1] = a;
  }

  return elf_hash(key, sizeof(int)*2);
}
