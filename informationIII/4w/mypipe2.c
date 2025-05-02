#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

int main(void)
{
    int fd, pipe_fd[2];
    char buf[128];
    int childpid, status;
    int newd;
    fd = open("mypipe2.c", O_RDONLY);

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }
    if ((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }
    else
    {
        if (childpid != 0)
        {
            close(0);
            newd = dup(fd);
            read(0, buf, sizeof(buf));
            write(pipe_fd[1], buf, sizeof(buf));
            wait(&status);
        }
        else
        {
            read(pipe_fd[0], buf, sizeof(buf));
            write(2, buf, sizeof(buf));
        }
    }
    return 0;
}