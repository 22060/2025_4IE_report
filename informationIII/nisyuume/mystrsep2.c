#include <stdio.h>
#include <string.h>

int main(void)
{
    char str[BUFSIZ];
    char *p = str;
    char *buf;
    char *strs[BUFSIZ];
    int i = 0;

    fgets(str, sizeof(str), stdin);
    str[strlen(str) - 1] = 0; // Remove newline character
    while ((buf = strsep(&p, " ")) != NULL)
    {
        strs[i++] = buf;
    }
    strs[i] = NULL; // Null-terminate the array
    while (**strs != NULL)
    {
        printf("%s\n", (*strs));
        (*strs) += strlen(*strs) + 1;
    }
    return 0;
}