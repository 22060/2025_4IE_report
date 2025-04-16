#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int main()
{
    char buf[BUFSIZ];

    int childpid;
    int status;
    char *arg1 = NULL;
    char *strs[BUFSIZ];
    int i = 0;
    while (1)
    {
        printf("prompt>");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = 0;
        char *p = buf;
        for (int j = 0; j < BUFSIZ; j++)
        {
            strs[j] = NULL;
        }
        while ((arg1 = strsep(&p, " ")) != NULL)
        {
            strs[i] = arg1;
            i++;
        }
        childpid = fork();
        if (childpid == 0)
        {
            printf("%d\n", getpid());
            sleep(10);
            exit(execv(strs[0], strs));
        }
        wait(&status);
        printf("%04x\n", status);
        i = 0;
    }
}