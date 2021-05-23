#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "sockets.h"

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

char *dt_str();

int main(void)
{
    int s, t, len;
    struct sockaddr_un remote;
    char str[100];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    printf("Connecting to [%s]\n", SOCK_PATH);

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("Connected to [%s]\n", SOCK_PATH);
    struct timespec ts_ms = timespec_ms(100);

    while (1)
    {
        char *time = dt_str();

        // create message TIME|<timestamp>
        char msg[strlen(time) + 5];
        strcpy(msg, "TIME");
        strcat(msg, SOH);
        strcat(msg, time);

        // ping TIME msg
        if (send(s, msg, strlen(msg), 0))
        {
            perror("send");
        }

        // wait...
        printf("Sending: %s", msg);
        nanosleep(&ts_ms, &ts_ms);
    }

    close(s);

    return 0;
}

char *dt_str()
{
    time_t t;
    time_t x_t = time(&t);

    struct tm *tm = gmtime(&x_t);

    return asctime(tm);
}
