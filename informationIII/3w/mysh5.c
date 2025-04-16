#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#define SIGSTOP 19
#define SIGCONT 18
int main()
{
    char buf[BUFSIZ];

    int childpid;
    int status;
    char *arg1 = NULL;
    char *strs[BUFSIZ];
    int i = 0;
    signal(SIGINT, SIG_IGN);
    while (1)
    {
        printf("%s>", getcwd(NULL, 0));
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
        if (strcmp(strs[0], "cd") == 0)
        {
            chdir(strs[1] == NULL ? getenv("HOME") : strs[1]);
            childpid = -1;
        }
        else
        {
            childpid = fork();
        }
        if (childpid == 0)
        {
            if (strcmp(strs[0], "k") == 0)
            {
                kill(atoi(strs[1]), SIGINT);
                exit(0);
            }
            else if (strcmp(strs[0], "s") == 0)
            {
                kill(atoi(strs[1]), SIGSTOP);
                exit(0);
            }
            else if (strcmp(strs[0], "c") == 0)
            {
                kill(atoi(strs[1]), SIGCONT);
                exit(0);
            }
            exit(execv(strs[0], strs));
        }
        wait(&status);
        printf("%04x\n", status);
        i = 0;
    }
}