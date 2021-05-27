// #include <ctype.h>
#include <pthread.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

void* get_a_warning(void* c) {
  int count = (int)c;
  return (void*)(count + 1);
}

int main(int argc, char** argv) {
  pthread_t t1;
  pthread_create(&t1, NULL, get_a_warning, (void*)1);

  void* rt1;
  pthread_join(t1, &rt1);

  int rti1 = (int)rt1;
  printf("Return value: %d\n", rti1);

  return 0;
}
