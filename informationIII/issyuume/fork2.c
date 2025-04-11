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
        sleep(5);
        printf("childpid");
        printf("childpid:%d\n", childpid);
        printf("pid:%d\n", getpid());
        printf("parent pid:%d\n", getppid());
        exit(0);
    }
    else
    {
        sleep(5);
        printf("parent\n");
        printf("childpid:%d\n", childpid);
        printf("pid:%d\n", getpid());
        printf("parent pid:%d\n", getppid());
        wait(&status);
    }
}