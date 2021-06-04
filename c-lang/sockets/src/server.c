#include "server.h"

#include <assert.h>
#include <sys/wait.h>

int current_array_size = 0;
int sock_fd = -1;
int connected_client_count = 0;
int* client_fds;

struct sockaddr_un local, remote;

void print_conn_array() {
  printf("Current connection array:\n");
  for (int i = 0; i < current_array_size; i++) {
    printf("Client #%d fd is %d\n", i + 1, client_fds[i]);
  }
}

void sig_chld_handler(int signo) {
  if (signo == SIGCHLD) {
    int status;
    wait(&status);

    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      printf("Exit status of the child was %d\n", exit_status);
      remove_client(exit_status);
    }
  }
}

void remove_client(int client_fd) {
  printf("remove_client %d\n", client_fd);
  print_conn_array();

  // zero out the fd in client_fds
  for (int i = 0; i < current_array_size; i++) {
    if (client_fds[i] == client_fd) {
      printf("Zeroing client_fd %d\n", client_fd);
      client_fds[i] = 0;
      break;
    }
  }

  // decrement connected_clients
  printf("Decrementing client count from %d\n", connected_client_count);
  connected_client_count--;

#ifndef NDEBUG
  int non_zero_clients = 0;
  for (int i = 0; i < current_array_size; i++) {
    if (client_fds[i] != 0) {
      non_zero_clients++;
    }
  }
  assert(non_zero_clients == connected_client_count);
#endif
  // ensure array size
  ensure_array_size();
}

void add_client(int client_fd) {
  printf("add_client %d\n", client_fd);

  // add fd to array in first available (zero) spot
  for (int i = 0; i < current_array_size; i++) {
    if (client_fds[i] == 0) {
      client_fds[i] = client_fd;
      printf("Added client fd %d at idx %d\n", client_fd, i);
      break;
    }
    // client_fds[connected_client_count] = client_fd;
  }

  // increment client count
  connected_client_count++;
  print_conn_array();

#ifndef NDEBUG
  int non_zero_clients = 0;
  for (int i = 0; i < current_array_size; i++) {
    if (client_fds[i] != 0) {
      non_zero_clients++;
    }
  }
  printf("non zero count: %d, connected count: %d\n", non_zero_clients,
         connected_client_count);
  assert(non_zero_clients == connected_client_count);
#endif

  // ensure array
  ensure_array_size();
}

void ensure_array_size() {
  if ((connected_client_count + 1) >=
          current_array_size  // we're close to the threshold
      || connected_client_count <=
             (current_array_size - 5)  // we're well under the threshold
  ) {
    // calculate new size
    int new_size = connected_client_count + 2;

    printf("%s array from %d to %d; %d connections\n",
           new_size > current_array_size ? "Growing" : "Shrinking",
           current_array_size, new_size, connected_client_count);

    // mallocing instead of reallocing so we can shift the 0 ones
    int* new_client_fds = (int*)malloc(sizeof(int) * new_size);
    memset(new_client_fds, 0, sizeof(int) * new_size);

#ifndef NDEBUG
    int non_zero_clients = 0;
    for (int i = 0; i < current_array_size; i++) {
      if (client_fds[i] != 0) {
        non_zero_clients++;
      }
    }
    assert(non_zero_clients == connected_client_count);
#endif

    ///
    int idx = 0;
    for (int i = 0; i < current_array_size; i++) {
      if (client_fds[i] != 0) {
        printf("Copying fd %d to new array\n", client_fds[i]);
        new_client_fds[idx] = client_fds[i];
        idx++;
      }
    }
    free(client_fds);             // free the original stuff
    client_fds = new_client_fds;  // repoint
    current_array_size = new_size;

  } else {
    printf("ensure array size doing nothing, connected: %d, size: %d\n",
           connected_client_count, current_array_size);
  }

  print_conn_array();
}

/**
 *
 */
int start_server() {
  // struct sockaddr_un local, remote;

  // initialise memory for the client file descriptors
  ensure_array_size();

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

  signal(SIGINT, sig_handler);
  signal(SIGCHLD, sig_chld_handler);

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
      // failed to fork()
      perror("fork");
      return EXIT_FAILURE;

    } else if (cpid == 0) {
      // inside the child proc
      // handler for the newly created socket.
      handle_connection(remote_sock_fd);

      // returning the fd of the closed socket
      return remote_sock_fd;

    } else {
      // inside the parent process
      add_client(remote_sock_fd);
    }
  }
}

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("received SIGINT %d\n", signo);

    for (int i = 0; i < connected_client_count; i++) {
      int fd = client_fds[i];
      if (fd != -1) {
        printf("Closing fd %d\n", client_fds[i]);
        close(client_fds[i]);

        connected_client_count--;
        client_fds[i] = 0;
      }
    }

    free(client_fds);

    // shut down listening socket?
    // close(sock_fd);
    shutdown(sock_fd, SHUT_RDWR);
    unlink(local.sun_path);
  }
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
      // is_error = 1;  // todo this is not is_error, just like should_continue
      // ?
    }

  } while (!is_error);

  printf("Exiting child for remote_sock_fd %d\n", remote_sock_fd);
  close(remote_sock_fd);
  return EXIT_SUCCESS;
}

int handle_command(int sock_fd, struct Command* command) {
  printf("Got command from %d -> %s\n", sock_fd, command->request);
  int is_error = 0;
  if (strlen(command->request) == strlen(CMD_PUB_TIME) &&
      strcmp(command->request, CMD_PUB_TIME) == 0) {
    // do something

    struct timespec ts_ms = timespec_ms(2000);
    while (is_error == 0) {
      // sleep
      nanosleep(&ts_ms, &ts_ms);

      char* time = dt_str();

      // create message TIME|<timestamp>
      char msg[strlen(time) + 5];
      strcpy(msg, "TIME");
      strcat(msg, SOH);
      strcat(msg, time);

      // ping TIME msg
      // printf("Sending to %d: %s\n", sock_fd, msg);
      int res = send(sock_fd, msg, strlen(msg), MSG_NOSIGNAL);
      // printf("Send res %d\n", res);
      if (res == -1) {
        perror("send");
        is_error = 1;
      }
    }

    printf("Left Command-handling while loop for fd %d\n", sock_fd);

  } else {
    printf("[%s] and [%s] NOT matched\n", command->request, CMD_PUB_TIME);
    printf("Unrecognised command; closing connection\n");
    close(sock_fd);
  }
  return EXIT_SUCCESS;
}
