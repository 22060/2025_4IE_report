#include <time.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char buf[BUFSIZ];
    time_t tval;
    struct tm *pTime;
    tval = time(NULL);
    pTime = localtime(&tval);

    strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
    printf("%s\n", buf);
    return 0;
}