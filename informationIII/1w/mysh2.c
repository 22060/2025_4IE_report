#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char buf[BUFSIZ];

    int childpid;
    int status;
    while (1)
    {
        printf("prompt>");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = 0; // Remove newline character
        childpid = fork();
        if (childpid == 0)
        {
            printf("%s\n", buf);
            execl(buf, buf, NULL);
        }
        wait(&status);
    }
}