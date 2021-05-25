#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* upcase(void* n) {
  char* name = (char*)n;

  printf("Original name: %s", name);

  char* upcased = malloc(strlen(name));

  for (int i = 0; i < strlen(name) - 1; i++) {
    upcased[i] = toupper(name[i]);
  }

  return (void*)upcased;
}

int main(int argc, char** argv) {
  char* name = "richard\n";

  pthread_t th;
  pthread_create(&th, NULL, upcase, (void*)name);

  void* ret_from_thread;
  char* ri;
  pthread_join(th, &ret_from_thread);
  ri = (char*)ret_from_thread;

  printf("Output from thread: %s\n", ri);

  printf("ret_from %p\n", ret_from_thread);
  printf("ri %p\n", ri);

  free(ret_from_thread);
  ret_from_thread = NULL;
  ri = NULL;
  return 0;
}
