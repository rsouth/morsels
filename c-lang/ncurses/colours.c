#include <ncurses.h>

void colouring();

int main(void)
{
    initscr();

    addstr("-----------------\n| codedrome.com |\n| ncurses Demo  |\n-----------------\n\n");
    refresh();

    //printing();

    //moving_and_sleeping();

    colouring();

    addstr("\npress any key to exit...");
    refresh();

    getch();

    endwin();

    return 0;
}

//--------------------------------------------------------
// FUNCTION colouring
//--------------------------------------------------------
void colouring()
{
    puts("1\n");
    if (has_colors())
    {
        puts("2\n");
        if (start_color() == OK)
        {
            puts("3\n");
            init_pair(1, COLOR_YELLOW, COLOR_RED);
            init_pair(2, COLOR_GREEN, COLOR_GREEN);
            init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

            attrset(COLOR_PAIR(1));
            addstr("Yellow and red\n\n");
            refresh();
            attroff(COLOR_PAIR(1));

            attrset(COLOR_PAIR(2) | A_BOLD);
            addstr("Green and green A_BOLD\n\n");
            refresh();
            attroff(COLOR_PAIR(2));
            attroff(A_BOLD);

            attrset(COLOR_PAIR(3));
            addstr("Magenta and cyan\n");
            refresh();
            attroff(COLOR_PAIR(3));
        }
        else
        {
            addstr("Cannot start colours\n");
            refresh();
        }
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }
}
