#include <pthread.h>
#include <stdio.h>

void* add_and_return(void* l) {
  long val = (long)l;
  printf("Thread input value: %ld\n", val);
  return (void*)(val + 100);
}

int main(int argc, char** argv) {
  long initial_value = 1;

  pthread_t t1;
  pthread_create(&t1, NULL, add_and_return, (void*)initial_value);

  void* returned;
  pthread_join(t1, &returned);

  //   long updated_value = (long)returned;

  printf("Returned value: %ld\n", (long)returned);

  return 0;
}
