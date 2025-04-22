#include "libmemes.h"
#include <7080S.H>

#define printf ((int (*)(const char *, ...))0x00007c7c)
#define scanf ((int (*)(const char *, ...))0x00007cb8)
int main(void)
{
    while (1)
    {
        printf("Hello, World!\n");
    }
    return 0;
}
