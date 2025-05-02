#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#define SIGSTOP 19
#define SIGCONT 18
int childpid1;
int childpid2;
void chtcher()
{
    kill(childpid1, SIGINT);
    if (childpid2 != -1)
    {
        kill(childpid2, SIGINT);
    }
}
void childlen(char *strs[BUFSIZ])
{
    if (strcmp(strs[0], "k") == 0)
    {
        exit(kill(atoi(strs[1]), SIGINT));
    }
    else if (strcmp(strs[0], "s") == 0)
    {
        exit(kill(atoi(strs[1]), SIGSTOP));
    }
    else if (strcmp(strs[0], "c") == 0)
    {
        exit(kill(atoi(strs[1]), SIGCONT));
    }
    exit(execvp(strs[0], strs));
}
int main()
{
    char buf[BUFSIZ];
    char buf2[BUFSIZ];

    int status1;
    int status2;
    char *arg1 = NULL;
    char *strs[BUFSIZ];
    int i = 0;
    int pipe_fd[2];
    int pipeflag = 0;
    int pipeindex = 0;
    signal(SIGINT, chtcher);
    while (1)
    {
        // get command and args
        printf("\x1b[36m%s\x1b[39m$", getcwd(NULL, 0));
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = 0;
        char *p = buf;
        // buf initialization
        childpid2 = -1;
        pipeflag = 0;
        for (int j = 0; j < BUFSIZ; j++)
        {
            strs[j] = NULL;
        }
        // parse command and args
        while ((arg1 = strsep(&p, " ")) != NULL)
        {
            strs[i] = arg1;
            if (strcmp(strs[i], "|") == 0)
            {
                pipeflag = 1;
                pipeindex = i;
                strs[i] = NULL;
                if (pipe(pipe_fd) == -1)
                {
                    perror("pipe");
                    exit(1);
                }
            }
            i++;
        }
        // check cd command
        if (strcmp(strs[0], "cd") == 0)
        {
            chdir(strs[1] == NULL ? getenv("HOME") : strs[1]);
            childpid1 = -1;
        }
        else
        {
            childpid1 = fork();
            if (pipeflag == 1)
            {
                if (childpid1 == 0)
                {
                    close(1);
                    close(pipe_fd[0]);
                    dup2(pipe_fd[1], 1);
                    childlen(strs);
                }
                else
                {
                    childpid2 = fork();
                    if (childpid2 == 0)
                    {
                        close(0);
                        close(pipe_fd[1]);
                        dup2(pipe_fd[0], 0);

                        if (strcmp(strs[pipeindex + 1], "ls") == 0)
                        {
                            strcpy(buf2, "--color=always");
                            strs[i] = buf2;
                        }
                        else if (strcmp(strs[pipeindex + 1], "grep") == 0)
                        {
                            strcpy(buf2, "--color=always");
                            strs[i] = buf2;
                        }
                        childlen(&(strs[pipeindex + 1]));
                    }
                    else
                    {
                        close(pipe_fd[0]);
                        close(pipe_fd[1]);
                    }
                }
            }
            else
            {
                if (childpid1 == 0)
                {
                    if (strcmp(strs[0], "ls") == 0)
                    {
                        strcpy(buf2, "--color=always");
                        strs[i] = buf2;
                    }
                    else if (strcmp(strs[0], "grep") == 0)
                    {
                        strcpy(buf2, "--color=always");
                        strs[i] = buf2;
                    }
                    childlen(strs);
                }
            }
        }
        // fork process

        waitpid(childpid1, &status1, 0);
        waitpid(childpid2, &status2, 0);
        printf("%04x\n", status1);
        printf("%04x\n", status2);
        i = 0;
    }
}