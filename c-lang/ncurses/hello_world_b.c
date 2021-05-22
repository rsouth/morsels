
/*

  CURHELLO.C
  ==========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  "Hello, world!", ncurses style.

*/

#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h> /*  for sleep()  */
#include <ncurses.h>
#include <string.h>

NCURSES_EXPORT(WINDOW *)
initscr(void);

int main(void)
{

    WINDOW *mainwin;

    /*  Initialize ncurses  */

    if ((mainwin = initscr()) == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    raw(); /* Line buffering disabled, Pass on
				 * everty thing to me 		*/
    noecho();

    // colours
    if (has_colors() && start_color() == OK)
    {
        init_pair(1, COLOR_YELLOW, COLOR_RED);
        init_pair(2, COLOR_GREEN, COLOR_GREEN);
        init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

        attrset(COLOR_PAIR(1));
        addstr("Yellow and red\n\n");
        refresh();
        attroff(COLOR_PAIR(1));
    }
    else
    {
        puts("Something went wrong...\n");
        return -1;
    }

    /*  Display "Hello, world!" in the centre of the
	screen, call refresh() to show our changes, and
	sleep() for a few seconds to get the full screen effect  */

    // void getmaxyx(WINDOW *win, int y, int x);
    int x, y;
    getmaxyx(mainwin, y, x);

    char *text = "Hello, ncurses!";
    int len = strlen(text);

    // mvaddstr(y / 2, (x / 2) - (len / 2), text);
    mvprintw(y / 2, x / 2 - (len / 2), text);

    refresh();
    // sleep(3);

    /*  Clean up after ourselves  */

    /////////////////////////////////////////////////
    char list[5][7] = {"One", "Two", "Three", "Four", "Five"};
    char item[7];
    int ch, i = 0, width = 7;
    initscr();          // initialize Ncurses
    box(mainwin, 0, 0); // sets default borders for the window
    // now print all the menu items and highlight the first one
    for (i = 0; i < 5; i++)
    {
        if (i == 0)
        {
            wattron(mainwin, A_STANDOUT); // highlights the first item.
            wattron(mainwin, A_BOLD);
            wattron(mainwin, COLOR_PAIR(1));
        }
        else
        {
            wattroff(mainwin, A_STANDOUT);
            wattroff(mainwin, A_BOLD);
            wattroff(mainwin, COLOR_PAIR(1));
        }
        sprintf(item, "%-6s", list[i]);
        mvwprintw(mainwin, i + 1, 2, "%s", item);
    }
    wrefresh(mainwin); // update the terminal screen
    i = 0;
    noecho();              // disable echoing of characters on the screen
    keypad(mainwin, TRUE); // enable keyboard input for the window.
    curs_set(0);           // hide the default screen cursor.
    // get the input
    while ((ch = wgetch(mainwin)) != 'q')
    {
        // right pad with spaces to make the items appear with even width.
        sprintf(item, "%-6s", list[i]);
        mvwprintw(mainwin, i + 1, 2, "%s", item);
        // use a variable to increment or decrement the value based on the input.
        switch (ch)
        {
        case KEY_UP:
        {
            i--;
            i = (i < 0) ? 4 : i;
            break;
        }
        case KEY_DOWN:
        {
            i++;
            i = (i > 4) ? 0 : i;
            break;
        }
        }
        // now highlight the next item in the list.
        wattron(mainwin, A_STANDOUT);
        wattron(mainwin, COLOR_PAIR(1));

        sprintf(item, "%-6s", list[i]);
        mvwprintw(mainwin, i + 1, 2, "%s", item);
        wattroff(mainwin, A_STANDOUT);
        wattroff(mainwin, COLOR_PAIR(1));
    }

    /////////////////////////////////////////////////

    // getch();
    // endwin();

    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}
