#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *strptime(const char *__restrict __s,
               const char *__restrict __fmt,
               struct tm *__tp);

time_t time(time_t *__timer) __THROW;

void parse_dt();

void current_time();

int main(void)
{
    parse_dt();
    current_time();

    return 0;
}

void parse_dt()
{
    printf("Parsing dates 📅 and times 🕑\n");

    // parsing date and time from string
    struct tm tm;
    char *s = strptime("2012-02-22 14:43:00", "%Y-%m-%d %H:%M:%S", &tm);
    if (s == NULL)
    {
        printf("Cannot parse date");
        return;
    }

    // print out the component parts from tm
    printf("year: %d; month: %d; day: %d;\n", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday);
    printf("hour: %d; minute: %d; second: %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("week day: %d; year day: %d\n", tm.tm_wday, tm.tm_yday);
}

void current_time()
{
    // current system time
    // time_t is a long int
    time_t t;
    time_t x = time(&t);

    printf("Since epoch: %ld\n", x);
}
