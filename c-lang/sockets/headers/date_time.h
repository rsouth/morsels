#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <time.h>

char* dt_str();

int nanosleep(const struct timespec* rqtp, struct timespec* rmtp);

struct timespec timespec_ms(int ms);

#endif
