#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "sockets.h"

// int nanosleep(const struct timespec* rqtp, struct timespec* rmtp);

// char* dt_str();

struct Msg parse_msg(char* inp) {
  struct Msg msg;
  char* tok = strtok(inp, SOH);
  strcpy(msg.type, tok);
  tok = strtok(NULL, SOH);
  strcpy(msg.data, tok);
  return msg;
}

int main(void) {
  int sock_fd;
  if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return EXIT_FAILURE;
  }

  printf("Connecting to [%s]\n", SOCK_PATH);

  struct sockaddr_un remote;
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, SOCK_PATH);
  int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  if (connect(sock_fd, (struct sockaddr*)&remote, len) == -1) {
    perror("connect");
    return EXIT_FAILURE;
  }

  printf("Connected to [%s]\n", SOCK_PATH);

  printf("Sending command: %s\n", CMD_PUB_TIME);
  // char* fake = "fake\n";

  // send(sock_fd, fake, strlen(fake), 0);
  send(sock_fd, CMD_PUB_TIME, strlen(CMD_PUB_TIME), 0);
  // if (send(sock_fd, CMD_PUB_TIME, strlen(CMD_PUB_TIME), 0)) {
  //   perror("send");
  //   return EXIT_FAILURE;
  // }

  int is_error = 0, read_count;
  char str[100];
  do {
    // wait for data on the socket (blocking)
    read_count = recv(sock_fd, str, 100, 0);
    if (read_count <= 0) {
      perror("recv");
      is_error = 1;
    }

    // parse and handle the message
    if (!is_error) {
      struct Msg message = parse_msg(str);
      printf("%s -> %s", message.type, message.data);
      // handle_command(&cmd);
    }

    // printf("is_error = %d", is_error);

  } while (!is_error);

  //   struct timespec ts_ms = timespec_ms(100);
  //   while (1) {
  //     char* time = dt_str();

  //     // create message TIME|<timestamp>
  //     char msg[strlen(time) + 5];
  //     strcpy(msg, "TIME");
  //     strcat(msg, SOH);
  //     strcat(msg, time);

  //     // ping TIME msg
  //     if (send(sock_fd, msg, strlen(msg), 0)) {
  //       perror("send");
  //     }

  //     // sleep
  //     printf("Sending: %s", msg);
  //     nanosleep(&ts_ms, &ts_ms);
  //   }

  //   close(sock_fd);
  //   return EXIT_SUCCESS;
}

// char* dt_str() {
//   time_t t;
//   time_t x_t = time(&t);

//   struct tm* tm = gmtime(&x_t);

//   return asctime(tm);
// }
