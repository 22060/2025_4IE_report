#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void chatcher()
{
    printf("caught\n");
    exit(0);
}

int main(void)
{
    signal(3, chatcher);
    while (1)
    {
        printf("y\n");
    }
    return 0;
}