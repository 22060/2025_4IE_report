#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void chatcher()
{
    printf("caught\n");
}
void chatcher2()
{
    printf("caught2\n");
    exit(0);
}
void chatcher3()
{
    printf("caught3\n");
}

void yes(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    if (argc == 1)
    {
        // printf("y");
    }
    // printf("\n");
}

int main(int argc, char *argv[])
{
    signal(2, chatcher);
    signal(3, chatcher2);
    signal(9, chatcher3);
    while (1)
    {
        yes(argc, argv);
    }
    return 0;
}