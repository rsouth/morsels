#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <time.h>

#include "sockets.h"

int connected_client_count = 0;
int client_fds[5];

int nanosleep(const struct timespec* rqtp, struct timespec* rmtp);

char* dt_str() {
  time_t t;
  time_t x_t = time(&t);

  struct tm* tm = gmtime(&x_t);

  return asctime(tm);
}

struct Command parse_command(char* input) {
  struct Command cmd;
  strcpy(cmd.request, input);
  return cmd;
}

int handle_command(int sock_fd, struct Command* command) {
  printf("Got command from %d -> %s\n", sock_fd, command->request);
  int is_error = 0;
  if (strlen(command->request) == strlen(CMD_PUB_TIME) &&
      strcmp(command->request, CMD_PUB_TIME) == 0) {
    // do something

    struct timespec ts_ms = timespec_ms(2000);
    while (is_error == 0) {
      char* time = dt_str();

      // create message TIME|<timestamp>
      char msg[strlen(time) + 5];
      strcpy(msg, "TIME");
      strcat(msg, SOH);
      strcat(msg, time);

      // ping TIME msg
      printf("Sending to %d: %s\n", sock_fd, msg);
      int res = send(sock_fd, msg, strlen(msg), MSG_NOSIGNAL);
      printf("Send res %d\n", res);
      if (res == -1) {
        perror("send");
        is_error = 1;
      }

      // sleep
      nanosleep(&ts_ms, &ts_ms);
    }

    printf("Left while loop for fd %d\n", sock_fd);

  } else {
    printf("[%s] and [%s] NOT matched\n", command->request, CMD_PUB_TIME);
    printf("Unrecognised command; closing connection\n");
    close(sock_fd);
  }
  return EXIT_SUCCESS;
}

int handle_connection(int remote_sock_fd) {
  int is_error = 0, read_count;
  char str[100];
  do {
    // wait for data on the socket (blocking)
    printf("recv on %d\n", remote_sock_fd);
    read_count = recv(remote_sock_fd, str, 100, 0);
    if (read_count <= 0) {
      perror("recv");
      is_error = 1;
    }

    // parse and handle the message
    if (!is_error) {
      struct Command cmd = parse_command(str);
      printf("Got Command: %s\n", cmd.request);
      handle_command(remote_sock_fd, &cmd);
    }

  } while (!is_error);

  printf("Exiting child for sock %d\n", remote_sock_fd);
  close(remote_sock_fd);
  return EXIT_SUCCESS;
}

int sock_fd;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("received SIGINT %d\n", signo);

    for (int i = 0; i < connected_client_count; i++) {
      printf("Closing fd %d\n", client_fds[i]);
      close(client_fds[i]);

      connected_client_count--;
      client_fds[i] = -1;
    }

    // shut down listening socket?
    close(sock_fd);
    // unlink(sock_fd);
  }
}

int main(int argc, char* argv[]) {
  if (argc == 2) {
    printf("The argument supplied is %s\n", argv[1]);
    if (strcmp(argv[1], "-c") == 0) {
      printf("starting client\n");
    } else if (strcmp(argv[1], "-s") == 0) {
      printf("starting server\n");
    } else {
      printf("Invalid args\n");
      return EXIT_FAILURE;
    }
  } else if (argc > 2) {
    printf("Too many arguments supplied.\n");
  } else {
    printf("One argument expected.\n");
  }

  struct sockaddr_un local, remote;

  sock_fd = socket(AF_UNIX,      // specify `domain socket`
                   SOCK_STREAM,  // TCP (SOCK_DGRAM for UDP)
                   0);           // automatic protocol resolution
  if (!sock_fd) {
    perror("socket");
    return EXIT_FAILURE;
  }

  // set up the local socket and unlink the local path if it exists already
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, SOCK_PATH);
  unlink(local.sun_path);

  // attempt to bind on the socket
  int len = strlen(local.sun_path) + sizeof(local.sun_family);
  if (bind(sock_fd, (struct sockaddr*)&local, len) == -1) {
    perror("bind");
    return EXIT_FAILURE;
  }

  // after MAX_QUEUED_CONNECTIONS are queued, connections will be refused
  const int MAX_QUEUED_CONNECTIONS = 5;
  if (listen(sock_fd, MAX_QUEUED_CONNECTIONS) == -1) {
    perror("listen");
    return EXIT_FAILURE;
  }

  printf("Listening...\n");

  // signal(SIGINT, sig_handler);

  while (1) {
    // wait for connection - blocking call
    unsigned int t = sizeof(remote);
    int remote_sock_fd;
    if ((remote_sock_fd = accept(sock_fd, (struct sockaddr*)&remote, &t)) ==
        -1) {
      perror("accept");
      return EXIT_FAILURE;
    }

    int cpid = fork();

    if (cpid < 0) {
      // shat the bed
    } else if (cpid == 0) {
      // kiddie
      // handler for the newly created socket.
      // take a copy of the fd for the socket and ping shit at it.
      handle_connection(remote_sock_fd);

    } else {
      // parent process
      // go back to listening for more connections I guess?
      // client_fds

      client_fds[connected_client_count] = remote_sock_fd;
      connected_client_count += 1;

      printf("Client [%d] connected with fd [%d]\n", connected_client_count,
             remote_sock_fd);

      signal(SIGINT, sig_handler);
    }
  }

  return EXIT_SUCCESS;
}

struct Msg parse_msg(char* inp) {
  struct Msg msg;
  char* tok = strtok(inp, SOH);
  strcpy(msg.type, tok);
  tok = strtok(NULL, SOH);
  strcpy(msg.data, tok);
  return msg;
}
