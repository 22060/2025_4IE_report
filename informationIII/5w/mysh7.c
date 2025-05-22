#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <time.h>
#include <pwd.h>
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
char *returnkengen(long int mode)
{
    char *kengen = malloc(11);
    kengen[1] = ((mode / 256 % 2) == 0) ? '-' : 'r';
    kengen[2] = ((mode / 128 % 2) == 0) ? '-' : 'w';
    kengen[3] = ((mode / 64 % 2) == 0) ? '-' : 'x';
    kengen[4] = ((mode / 32 % 2) == 0) ? '-' : 'r';
    kengen[5] = ((mode / 16 % 2) == 0) ? '-' : 'w';
    kengen[6] = ((mode / 8 % 2) == 0) ? '-' : 'x';
    kengen[7] = ((mode / 4 % 2) == 0) ? '-' : 'r';
    kengen[8] = ((mode / 2 % 2) == 0) ? '-' : 'w';
    kengen[9] = ((mode % 2) == 0) ? '-' : 'x';
    kengen[10] = 0;
    kengen[0] = '-';
    return kengen;
}
void lsF(char *strs[BUFSIZ])
{
    char *dotav[2];
    dotav[0] = "./";
    dotav[1] = NULL;
    FTS *ftsp;
    FTSENT *p;
    int flag_l = 0;
    int flag_i = 0;
    int i = 0;
    char date[BUFSIZ];
    while (strs[i] != NULL)
    {
        if (strcmp(strs[i], "-l") == 0)
        {
            flag_l = 1;
        }
        else if (strcmp(strs[i], "-i") == 0)
        {
            flag_i = 1;
        }
        i++;
    }
    ftsp = fts_open(dotav, FTS_PHYSICAL, NULL);
    i = 0;
    while (((p = fts_read(ftsp)) != NULL))
    {
        switch (flag_i + flag_l * 2)
        {
        case 0:
            if (p->fts_info == FTS_F)
            {
                printf("%s ", p->fts_name);
            }
            break;
        case 1:
            if (p->fts_info == FTS_F)
            {
                i++;
                printf("%ld %s   ", p->fts_statp->st_ino, p->fts_name);
                if (i % 3 == 0)
                {
                    printf("\n");
                }
            }
            break;
        case 2:
            if (p->fts_info == FTS_F)
            {
                // 権限 ハードリンク 所有者 作成者 タイムスタンプ ファイル名
                strcpy(date, ctime(&p->fts_statp->st_mtime));
                date[strlen(date) - 1] = 0;
                struct passwd *pw1 = getpwuid(p->fts_statp->st_uid);
                printf("%s %ld %s  %s %s\n", returnkengen(p->fts_statp->st_mode), p->fts_statp->st_nlink, pw1 ? pw1->pw_name : "unknown", date, p->fts_name);
            }
            break;
        }
    }
    printf("\n");
}
int main()
{
    char buf[BUFSIZ];

    int status1;
    int status2;
    char *arg1 = NULL;
    char *strs[BUFSIZ];
    int i = 0;
    int pipe_fd[2];
    int pipe_fd2[2];
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
                if (strcmp(strs[pipeindex + 1], "grep") == 0)
                {
                    strs[i] = "--color=always";
                }
                if (childpid1 == 0)
                {
                    if (strcmp(strs[0], "ls-F") == 0)
                    {
                        if (pipe(pipe_fd2) == -1)
                        {
                            perror("pipe");
                            exit(1);
                        }
                        dup2(1, pipe_fd2[1]);
                        close(1);
                        close(pipe_fd[0]);
                        dup2(pipe_fd[1], 1);
                        lsF(strs);
                        dup2(pipe_fd2[1], 1);
                    }
                    else
                    {
                        close(1);
                        close(pipe_fd[0]);
                        dup2(pipe_fd[1], 1);
                        childlen(strs);
                    }
                }
                else
                {
                    childpid2 = fork();
                    if (childpid2 == 0)
                    {
                        close(0);
                        close(pipe_fd[1]);
                        dup2(pipe_fd[0], 0);

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
                if (strcmp(strs[0], "ls") == 0)
                {
                    strs[i] = "--color=always";
                }
                if (childpid1 == 0)
                {
                    if (strcmp(strs[0], "ls-F") == 0)
                    {
                        lsF(strs);
                    }
                    childlen(strs);
                }
            }
        }
        // fork process

        waitpid(childpid1, &status1, 0);
        waitpid(childpid2, &status2, 0);
        // printf("%04x\n", status1);
        // printf("%04x\n", status2);
        i = 0;
    }
}