#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define H 0.5
double f(double x, double y);
double fy(double x);
int main(void)
{
    double x0 = 0;
    double y0 = 0.5;
    double mem[2];
    mem[0] = x0;
    mem[1] = y0;
    for (int i = 0; i < 4; i++)
    {
        mem[1] += f(mem[0], mem[1]);
        mem[0] += H;
        printf("calc:x%d = %f, y%d = %f\n", i + 1, mem[0], i + 1, mem[1]);
        printf("true:y%d = %f\n", i + 1, fy(mem[0]));
    }
}
double fy(double x)
{
    return 0.5 * (1 + x) * (1 + x);
}
double f(double x, double y)
{
    return 2 * y / (1 + x) * H;
}