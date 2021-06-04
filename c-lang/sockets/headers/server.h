#ifndef SERVER_H
#define SERVER_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "date_time.h"
#include "messaging.h"
#include "sockets.h"

#define client_count_increment 2

void sig_handler(int signo);
void sig_chld_handler(int signo);

int start_server();

int handle_connection(int remote_sock_fd);

int handle_command(int sock_fd, struct Command* command);

struct Command parse_command(char* input);

void ensure_array_size();
void remove_client(int client_fd);
void add_client(int client_fd);

void print_conn_array();

#endif
