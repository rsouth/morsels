#include "date_time.h"

char* dt_str() {
  time_t t;
  time_t x_t = time(&t);

  struct tm* tm = gmtime(&x_t);

  return asctime(tm);
}

struct timespec timespec_ms(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  return ts;
}
