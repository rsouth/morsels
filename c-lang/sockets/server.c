#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <ncurses.h>
#include <fcntl.h>

#include "sockets.h"

/**
 * ncurses TUI, listens on a domain socket and displays any input
 * 
 * hit it with
 * ➜  sockets git:(main) ✗ echo TEST | socat - unix-connect:/gui_socket
*/

WINDOW *setup_ui();

int handle_time_msg(WINDOW *wnd, struct Msg msg);

struct Msg parse_msg(char *inp);

int main(void)
{
    int socket_fd, s2, len;
    unsigned int t;
    struct sockaddr_un local, remote;
    char str[100];

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0); // AF_UNIX for the Domain Socket?
                                                 // SOCK_STREAM is TCP-esque, as opposed to <WHAT IS IT?>
                                                 // which is UDP-like but more reliable

    if (!socket_fd)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path); // make sure it doesn't exist already...
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(socket_fd, (struct sockaddr *)&local, len) == -1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }

    const int MAX_QUEUED_CONNECTIONS = 5;
    if (listen(socket_fd, MAX_QUEUED_CONNECTIONS) == -1)
    {
        perror("listen");
        return EXIT_FAILURE;
    }

    //////////////////////////////////////////
    /////////////////////////////////////////
    ////// ======= SET UP UI ================
    // WINDOW *mainwin;

    // // Initialise ncurses
    // if ((mainwin = initscr()) == NULL)
    // {
    //     fprintf(stderr, "Error initialising ncurses.\n");
    //     return EXIT_FAILURE;
    // }

    // cbreak();               // don't process certain chars (e.g. return)
    // noecho();               // don't write out input to the screen, when getch gets a char
    // nodelay(mainwin, TRUE); // make getch non-blocking; getch will return ERR if no input.
    // curs_set(0);            // hide the cursor

    WINDOW *mainwin = setup_ui();
    if (mainwin == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        return EXIT_FAILURE;
    }

    int x, y;                // Get the width and height of the terminal area
    getmaxyx(mainwin, y, x); // Note that ncurses uses (y, x) rather than (x, y)

    move(0, 0);
    wprintw(mainwin, "Listening...");
    wrefresh(mainwin);

    // wait for connection
    t = sizeof(remote);
    if ((s2 = accept(socket_fd, (struct sockaddr *)&remote, &t)) == -1)
    {
        perror("accept");
        return EXIT_FAILURE;
    }

    move(0, 0);
    clrtoeol();
    wprintw(mainwin, "Client connected!");
    wrefresh(mainwin);

    // int current_char; // the latest input typed
    while (wgetch(mainwin) != 'q')
    {
        int done = 0, n;

        do
        {
            n = recv(s2, str, 100, 0);
            if (n <= 0)
            {
                if (n < 0)
                    perror("recv");
                done = 1;
            }

            if (!done)
            {
                struct Msg msg = parse_msg(str);
                if (strcmp(msg.type, "TIME|"))
                {
                    done = handle_time_msg(mainwin, msg);
                }
            }

            wrefresh(mainwin);
        } while (!done);

        close(s2);
    }

    return EXIT_SUCCESS;
}

struct Msg parse_msg(char *inp)
{
    struct Msg msg;
    char *tok = strtok(inp, SOH);
    strcpy(msg.type, tok);
    tok = strtok(NULL, SOH);
    strcpy(msg.data, tok);
    return msg;
}

int handle_time_msg(WINDOW *wnd, struct Msg msg)
{
    int x, y; // Get the width and height of the terminal area
    getmaxyx(wnd, y, x);
    mvprintw((y / 2), (x / 2) - (strlen(msg.data) / 2), "%s", msg.data);

    return 0;
}

WINDOW *setup_ui()
{
    WINDOW *mainwin;

    // Initialise ncurses
    if ((mainwin = initscr()) == NULL)
    {
        return NULL;
    }

    cbreak();               // don't process certain chars (e.g. return)
    noecho();               // don't write out input to the screen, when getch gets a char
    nodelay(mainwin, TRUE); // make getch non-blocking; getch will return ERR if no input.
    curs_set(0);            // hide the cursor

    return mainwin;
}
