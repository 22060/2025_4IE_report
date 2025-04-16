#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void chatcher()
{
    printf("caught\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(2, chatcher);
    while (1)
    {
        for (int i = 1; i < argc; i++)
        {
            printf("%s ", argv[i]);
        }
        if (argc == 1)
        {
            printf("y");
        }
        printf("\n");
    }
    return 0;
}