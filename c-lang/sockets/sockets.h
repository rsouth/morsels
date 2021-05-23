#ifndef SOCKETS_H
#define SOCKETS_H

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

#include <time.h>

#define SOCK_PATH "gui.socket"

const char *SOH = "|";

struct Msg
{
    char type[5];
    char data[100];
};

struct timespec timespec_ms(int ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    return ts;
}

#endif
