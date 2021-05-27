#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* incrementing(void* n) {
  long* l = (long*)n;
  long ll = *l;
  printf("got: %lu\n", ll);

  ll++;
  *l = ll;
  return (void*)NULL;
}

int main(int argc, char** argv) {
  int THREADS = 5;
  long counter = 0;

  printf("initial value: %lu\n", counter);

  pthread_t threads[THREADS];

  for (int i = 0; i < THREADS; i++) {
    pthread_create(&threads[i], NULL, incrementing, &counter);
  }

  for (int j = 0; j < THREADS; j++) {
    pthread_join(threads[j], NULL);
  }

  printf("Total is: %lu\n", counter);
  return 0;
}
