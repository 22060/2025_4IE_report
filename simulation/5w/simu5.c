#include <stdio.h>
#include <string.h>
#include <stdlib.h>
double f1(double x, double y1, double y2);
double f2(double x, double y1, double y2);
double fy(double x);
double a = 2;
double b = 1.0;
double c = 3.0;
double d = 1.0;
double h = 0.01;
int main(void)
{
    double x0 = 0;
    double y0 = 0.5;
    double x = 0, y1 = 5.0, y2 = 1.0;
    double mem = 0.0;
    printf("# x,y1,y2\n");
    for (int i = 0; i < 5000; i++)
    {
        mem = y1 + h * f1(x, y1, y2);
        y2 += h * f2(x, y1, y2);
        y1 = mem;
        x += h;
        printf("%f %f %f\n", x, y1, y2);
    }
}
double fy(double x)
{
    return 0.5 * (1 + x) * (1 + x);
}
double f1(double x, double y1, double y2)
{
    return a * y1 - c * y1 * y2;
}
double f2(double x, double y1, double y2)
{
    return -1 * b * y2 + d * y1 * y2;
}