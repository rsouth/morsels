#include <stdio.h>
#include <stdlib.h>

// Project Euler
// -------------

void problem_1();

int main(void)
{
    problem_1();
    return 0;
}

/*
    Problem 1
    If we list all the natural numbers below 10 that are multiples of 3 or 5, 
    we get 3, 5, 6 and 9. The sum of these multiples is 23.

    Find the sum of all the multiples of 3 or 5 below 1000.
*/
void problem_1()
{
    for (int i = 1; i < 10; i++)
    {
        if (i % 3 == 0 || i % 5 == 0)
        {
            printf("%d\n", i);
        }
    }
}
