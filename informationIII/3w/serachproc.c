#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main(void)
{
    for (int i = 0; i < 100000; i++)
    {
        if (kill(i, 0) == 0)
        {
            printf("Process %d is running\n", i);
        }
    }
}