#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t lock;

void* incrementing(void* n) {
  sleep(1 + rand() % 5);
  return (void*)1;
}

int main(int argc, char** argv) {
  long counter = 0;

  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\n mutex init failed\n");
    return 1;
  }

  printf("initial value: %lu\n", counter);

  pthread_t threads[10];

  printf("Started %lu threads...\n", sizeof(threads) / sizeof(pthread_t));

  for (int i = 0; i < 10; i++) {
    pthread_create(&threads[i], NULL, incrementing, &counter);
  }

  long ccc = 0;
  for (int j = 0; j < 10; j++) {
    void* count;
    pthread_join(threads[j], &count);
    ccc += *(long*)count;
    printf("sum is: %lu\n", ccc);
  }
  printf("Total is: %lu\n", ccc);

  return 0;
}
