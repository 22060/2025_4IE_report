#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int childpid;
    int childpid2;
    int status;
    childpid = fork();
    if (childpid == 0)
    {
        childpid2 = fork();
        if (childpid2 == 0)
        {
            sleep(5);
            printf("child2\n");
            printf("childpid:%d\n", childpid2);
            printf("pid:%d\n", getpid());
            printf("parent pid:%d\n\n", getppid());
            exit(0);
        }
        printf("child\n");
        printf("childpid:%d\n", childpid2);
        printf("pid:%d\n", getpid());
        printf("parent pid:%d\n\n", getppid());
        sleep(5);
        exit(0);
    }
    else
    {
        sleep(6);
        printf("parent\n");
        printf("childpid:%d\n", childpid);
        printf("pid:%d\n", getpid());
        printf("parent pid:%d\n", getppid());
        wait(&status);
    }
}