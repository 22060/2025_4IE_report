#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(void)
{
    int fd, pipe_fd[2];
    char buf[140];
    fd = open("mypipe1.c", O_RDONLY);

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    read(fd, buf, sizeof(buf));
    write(pipe_fd[1], buf, sizeof(buf));
    read(pipe_fd[0], buf, sizeof(buf));
    write(1, buf, sizeof(buf));
    return 0;
}