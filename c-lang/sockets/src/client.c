// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <sys/un.h>

// #include "messaging.h"
// #include "sockets.h"
#include "client.h"

int start_client() {
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

  printf("Sending command: %s of len %lu\n", CMD_PUB_TIME,
         strlen(CMD_PUB_TIME));
  // char* fake = "fake\n";

  // send(sock_fd, fake, strlen(fake), 0);
  // send(sock_fd, CMD_PUB_TIME, strlen(CMD_PUB_TIME), 0);
  if (!send(sock_fd, CMD_PUB_TIME, strlen(CMD_PUB_TIME), 0)) {
    perror("send");
    return EXIT_FAILURE;
  }

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

  return EXIT_SUCCESS;
}
