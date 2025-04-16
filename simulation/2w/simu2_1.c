#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define H 0.5
double f(double x, double y);
int main(void)
{
    int x0 = 0;
    int y0 = 1;
    double mem[2];
    mem[0] = x0;
    mem[1] = y0;
    for (int i = 0; i < 4; i++)
    {
        mem[1] += f(mem[0], mem[1]);
        mem[0] += H;
        printf("x%d = %f, y%d = %f\n", i + 1, mem[0], i + 1, mem[1]);
    }
}
double f(double x, double y)
{
    return y / (1 + x) * H;
}