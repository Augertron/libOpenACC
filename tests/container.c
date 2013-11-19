
#include "OpenACC/utils/containers/associative.h"

#include <stdio.h>

int int_cmp(int * k1, int * k2) {
  return (*k1) < (*k2);
}

void print_container_int_int(struct associative_t_ * container) {
  size_t i;
  printf("container->count = \n", container->count);
  for (i = 0; i < container->count; i++) {
    printf("  container->key [%d]  = %d\n", i, *(int *)(container->datas + i * container->storage_size));
    printf("  container->data[%d] = %d\n", i, *(int *)(container->datas + i * container->storage_size + container->key_size));
  }
  printf("\n");
}

int main() {

  struct associative_t_ * container = associative_alloc(30, sizeof(int), sizeof(int), &int_cmp);

  int k1 = 1;
  int k2 = 2;
  int k3 = 3;
  int k4 = 4;
  int k5 = 5;
  int k6 = 6;
  int k7 = 7;
  int k8 = 8;
  
//  const int * associative_lookup(container, key);
//  associative_remove(container, key);
//  associative_insert(container, key, data);

  print_container_int_int(container);

  associative_insert(container, &k1, &k1);

  print_container_int_int(container);

  associative_insert(container, &k3, &k3);

  print_container_int_int(container);

  associative_insert(container, &k7, &k7);

  print_container_int_int(container);

  associative_insert(container, &k5, &k5);

  print_container_int_int(container);

  associative_insert(container, &k4, &k4);

  print_container_int_int(container);

  associative_insert(container, &k2, &k2);

  print_container_int_int(container);

  return 0;
}

