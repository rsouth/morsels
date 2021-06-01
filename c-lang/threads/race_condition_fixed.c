#include <pthread.h>
#include <stdio.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
volatile unsigned char data_ready_flag = 0;
volatile unsigned char running = 0;

/**
 * "Consumer", waits on the condition and upon receiving new date, printf's it.
 */
void* reader(void* n) {
  pthread_t t_id = pthread_self();
  int ms_pause = 10;
  struct timespec wait_time;
  wait_time.tv_nsec = ms_pause * 1000000;

  while (running) {
    pthread_mutex_lock(&m);
    pthread_cond_timedwait(&c, &m, &wait_time);

    if (data_ready_flag == 0) {
      pthread_mutex_unlock(&m);
      continue;
    }

    long l = *(long*)n;
    printf("[Consumer Thread %lu] Got signal & data is ready: %lu\n", t_id, l);
    data_ready_flag = 0;
    pthread_mutex_unlock(&m);
  }

  printf("[Consumer Thread %lu] Exiting consumer\n", t_id);
  return (void*)NULL;
}

/**
 * "Producer" increments the value and signals on the condition. Once the value
 * is equal to 10, it shuts down.
 */
void* writer(void* n) {
  pthread_t t_id = pthread_self();

  while (running) {
    pthread_mutex_lock(&m);

    if (data_ready_flag == 1) {
      pthread_mutex_unlock(&m);
      continue;
    }

    (*(long*)n)++;
    printf("[Producer Thread %lu] Incr to: %lu\n", t_id, *(long*)n);

    if (*(long*)n == 5) {
      running = 0;
      printf("[Producer Thread %lu] Set running %i\n", t_id, running);
    }

    data_ready_flag = 1;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
  }

  printf("[Producer Thread %lu] Exiting producer\n", t_id);
  return NULL;
}

int main(int argc, char** argv) {
  pthread_t t_producer;
  pthread_t t_consumer;
  long shared_data = 0;

  running = 1;
  pthread_create(&t_producer, NULL, writer, &shared_data);
  pthread_create(&t_consumer, NULL, reader, &shared_data);

  pthread_join(t_consumer, NULL);

  printf("Done with value %lu\n", shared_data);
  return 0;
}
