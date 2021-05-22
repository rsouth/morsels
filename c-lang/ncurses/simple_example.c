#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

char *dt_str();

int main(void)
{
    WINDOW *mainwin;

    // Initialise ncurses
    if ((mainwin = initscr()) == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    // raw();    // Disable line buffering, so the user doesn't have to hit return
    cbreak(); // don't process certain chars (e.g. return)
    // https://linux.die.net/man/3/raw
    // https://linux.die.net/man/3/cbreak

    noecho(); // don't write out input to the screen, when getch gets a char
    // https://linux.die.net/man/3/noecho

    // for non-blocking wgetch
    nodelay(mainwin, TRUE); // make getch non-blocking; getch will return ERR if no input.
    // https://linux.die.net/man/3/nodelay

    curs_set(0); // hide the cursor
    // https://linux.die.net/man/3/curs_set

    int x, y;                // Get the width and height of the terminal area
    getmaxyx(mainwin, y, x); // Note that ncurses uses (y, x) rather than (x, y)

    char *text = "Hello, ncurses!";                    // Draw out 'Hello...'
    mvprintw(y / 2, x / 2 - (strlen(text) / 2), text); // approximately centred

    char *prompt = "Press q to quit";
    mvprintw((y / 2) + 5, x / 2 - (strlen(prompt) / 2), prompt);

    int current_char;           // the latest input typed
    char last_valid_char = ERR; // the latest non-error input typed

    while ((current_char = wgetch(mainwin)) != 'q')
    {
        if (current_char != ERR)
        {
            last_valid_char = current_char;
        }

        // get + show the current date & time on-screen
        char *time = dt_str();
        mvprintw((y / 2) + 2, (x / 2) - (strlen(time) / 2), "%s", time);

        // output of latest key
        move((y / 2) + 3, 0); // move to the start of the line
        clrtoeol();           // and clear the whole line

        char *pak = "Last key: ";
        int pak_len = strlen(pak);
        mvprintw((y / 2) + 3, (x / 2) - (pak_len / 2), pak);
        if (last_valid_char != ERR)
        {
            int pos = (x / 2) + (pak_len / 2);
            mvprintw((y / 2) + 3, pos, "%c", last_valid_char);
        }

        wrefresh(mainwin);
    }

    // quitting; kill our window and shut down ncurses
    delwin(mainwin); // deletes the window (freeing memory)
    //https://linux.die.net/man/3/delwin

    endwin(); // make sure your terminal is properly restored
    // https://linux.die.net/man/3/endwin

    return EXIT_SUCCESS;
}

char *dt_str()
{
    time_t t;
    time_t x_t = time(&t);

    struct tm *tm = gmtime(&x_t);

    return asctime(tm);
}
