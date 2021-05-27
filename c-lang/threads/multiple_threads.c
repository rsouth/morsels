#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t lock;

void* upcase_working(void* n) {
  pthread_mutex_lock(&lock);

  uintptr_t x = (uintptr_t)n;
  sleep(rand() % 1);
  printf("Input: %c\n", (char)x);

  pthread_mutex_unlock(&lock);
  return (void*)(uintptr_t)toupper(x);
}

int main(int argc, char** argv) {
  char* name = "untitled1.dev\n";

  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\n mutex init failed\n");
    return 1;
  }

  pthread_t threads[strlen(name) - 1];

  for (int i = 0; i < strlen(name) - 1; i++) {
    pthread_create(&threads[i], NULL, upcase_working,
                   (void*)(uintptr_t)name[i]);
  }

  printf("Started %lu threads...\n", sizeof(threads) / sizeof(pthread_t));

  char chars[strlen(name)];
  for (int i = 0; i < strlen(name) - 1; i++) {
    void* rft;
    pthread_join(threads[i], &rft);

    chars[i] = (char)rft;
  }

  printf("output string: %s\n", chars);

  return 0;
}
