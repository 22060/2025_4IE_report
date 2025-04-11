#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    char buf[BUFSIZ];
    while (1)
    {
        printf("prompt>");
        fgets(buf, sizeof(buf), stdin);
        for (int i = 0; i < strlen(buf); i++)
        {
            printf("%c", buf[i]);
            if (buf[i] == '\n')
            {
                buf[i] = '\0';
            }
        }
    }
}