#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/unistd.h>

#include "sockets.h"

/**
 * ncurses TUI, listens on a domain socket and displays any input
 *
 * hit it with
 * sockets git:(main) ✗ echo -n "TIME|12345" | socat - unix-connect:/gui_socket
 */

WINDOW* setup_ui();

int handle_time_msg(WINDOW* wnd, struct Msg msg);

struct Msg parse_msg(char* inp);

int main(void) {
  struct sockaddr_un local, remote;

  int sock_fd = socket(AF_UNIX,      // specify `domain socket`
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

  // Set up ncurses UI
  WINDOW* mainwin = setup_ui();
  if (mainwin == NULL) {
    fprintf(stderr, "Error initialising ncurses.\n");
    return EXIT_FAILURE;
  }

  move(0, 0);
  wprintw(mainwin, "Listening...");
  wrefresh(mainwin);

  // wait for connection - blocking call
  unsigned int t = sizeof(remote);
  int remote_sock_fd;
  if ((remote_sock_fd = accept(sock_fd, (struct sockaddr*)&remote, &t)) == -1) {
    perror("accept");
    return EXIT_FAILURE;
  }

  move(0, 0);
  clrtoeol();
  wprintw(mainwin, "Client connected!");
  wrefresh(mainwin);

  int is_error = 0, read_count;
  char str[100];
  do {
    // wait for data on the socket (blocking)
    read_count = recv(remote_sock_fd, str, 100, 0);
    if (read_count <= 0) {
      perror("recv");
      is_error = 1;
    }

    // parse and handle the message
    if (!is_error) {
      struct Msg msg = parse_msg(str);
      if (strcmp(msg.type, "TIME|")) {
        is_error = handle_time_msg(mainwin, msg);
      }
    }

    // update ncurses
    wrefresh(mainwin);
  } while (!is_error);

  close(remote_sock_fd);
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

int handle_time_msg(WINDOW* wnd, struct Msg msg) {
  int x, y;  // Get the width and height of the terminal area
  getmaxyx(wnd, y, x);
  mvprintw((y / 2), (x / 2) - (strlen(msg.data) / 2), "%s", msg.data);

  return 0;
}

WINDOW* setup_ui() {
  WINDOW* mainwin;

  // Initialise ncurses
  if ((mainwin = initscr()) == NULL) {
    return NULL;
  }

  cbreak();
  noecho();
  nodelay(mainwin, TRUE);
  curs_set(0);

  return mainwin;
}
