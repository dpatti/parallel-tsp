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

  return h % 256;
}

unsigned edge_hash(int a, int b) {
  int key[2];
  if (a < b) {
    key[0] = a * 1234567;
    key[1] = b * 1234567;
  } else {
    key[0] = b * 1234567;
    key[1] = a * 1234567;
  }

  // XXX: This is a shitty way to prevent 0 returns
  return elf_hash(key, sizeof(int)*2) + 1;
}
