#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
    int childpid;
    int status;
    int count = 0;
    while (1)
    {
        sleep(2);
        if ((childpid = fork()) == 0)
        {
            count++;
            printf("child No.%d:\tmy pid = %d,parent pid = %d\n", count, getpid(), getppid());
        }
        else
        {
            printf("parent:\t\tmy pid = %d,child pid = %d\n", getpid(), getppid());
            wait(&status);
        }
    }
}