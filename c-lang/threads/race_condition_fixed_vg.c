#include <pthread.h>
#include <stdio.h>

// pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t c = PTHREAD_COND_INITIALIZER;
volatile unsigned char data_ready_flag = 0;
volatile unsigned char running = 0;

struct shr_data {
  pthread_mutex_t m;
  pthread_cond_t c;
  long shared_data;
};

/**
 * "Consumer", waits on the condition and upon receiving new date, printf's it.
 */
void* reader(void* n) {
  pthread_t t_id = pthread_self();
  int ms_pause = 10;
  struct timespec wait_time;
  wait_time.tv_nsec = ms_pause * 1000000;
  wait_time.tv_sec = ms_pause * 1000;

  struct shr_data* sd = (struct shr_data*)n;

  while (running) {
    pthread_mutex_lock(&sd->m);
    pthread_cond_timedwait(&sd->c, &sd->m, &wait_time);

    if (data_ready_flag == 0) {
      pthread_mutex_unlock(&sd->m);
      continue;
    }

    long l = sd->shared_data;
    printf("[Consumer Thread %lu] Got signal & data is ready: %lu\n", t_id, l);
    data_ready_flag = 0;
    pthread_mutex_unlock(&sd->m);
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

  struct shr_data* sd = (struct shr_data*)n;

  while (running) {
    pthread_mutex_lock(&sd->m);

    if (data_ready_flag == 1) {
      pthread_mutex_unlock(&sd->m);
      continue;
    }

    sd->shared_data++;
    printf("[Producer Thread %lu] Incr to: %lu\n", t_id, sd->shared_data);

    if (sd->shared_data == 5) {
      running = 0;
      printf("[Producer Thread %lu] Set running %i\n", t_id, running);
    }

    data_ready_flag = 1;
    pthread_cond_signal(&sd->c);
    pthread_mutex_unlock(&sd->m);
  }

  printf("[Producer Thread %lu] Exiting producer\n", t_id);
  return NULL;
}

int main(int argc, char** argv) {
  // long shared_data = 0;
  running = 1;

  struct shr_data sd;
  if (pthread_cond_init(&sd.c, NULL) + pthread_mutex_init(&sd.m, NULL) != 0) {
    printf("Error...");
  }
  sd.shared_data = 0;

  pthread_t t_producer;
  pthread_create(&t_producer, NULL, &writer, &sd);

  pthread_t t_consumer;
  pthread_create(&t_consumer, NULL, &reader, &sd);

  pthread_join(t_consumer, NULL);
  pthread_join(t_producer, NULL);

  // pthread_mutex_destroy(&sd.m);
  // pthread_cond_destroy(&sd.c);

  printf("Done with value %lu\n", sd.shared_data);
  return 0;
}
