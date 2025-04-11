#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int childpid;
    int status;
    childpid = fork();
    if (childpid == 0)
    {
        sleep(3);
        printf("echo %d\n", childpid);
        exit(0);
    }
    else
    {
        sleep(2);
        printf("parent:%d\n", childpid);
        wait(&status);
        printf("parent2\n");
    }
}