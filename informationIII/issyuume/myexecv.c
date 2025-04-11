#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    execv(argv[1], &argv[1]);
}