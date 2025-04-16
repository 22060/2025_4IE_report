#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[])
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
        char *ptr = buf;
        char *p = buf;
        while ((arg1 = strsep(&p, " ")) != NULL)
        {
            strs[i] = arg1;
            i++;
        }
        strs[i] = NULL;
        childpid = fork();
        if (childpid == 0)
        {
            execv(strsep(&ptr, " "), strs);
        }
        wait(&status);
        i = 0;
        for (int j = 0; j < BUFSIZ; j++)
        {
            strs[j] = NULL;
        }
    }
}