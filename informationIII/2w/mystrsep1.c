#include <stdio.h>
#include <string.h>

int main(void)
{
    char str[BUFSIZ] = "This is a pen.";
    char *p = str;
    char *buf;
    while ((buf = strsep(&p, " ")) != NULL)
    {
        printf("%s\n", buf);
    }
}