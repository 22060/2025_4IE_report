#include <stdio.h>
#include <string.h>
#include <stdlib.h>
double fy(double x, double y, double v);
double fv(double x, double y, double v);
double f(double x);
double m = 1.0;
double k = 2.0;
double l = 0.3;
double h = 0.1;
int main(void)
{
    double x0 = 0;
    double y0 = 0.5;
    double t = 0, y = 10.0, v = 0.0;
    double mem = 0.0;
    printf("# x,y,v\n");
    for (int i = 0; i < 100; i++)
    {
        mem = y + h * fy(t, y, v);
        v += h * fv(t, y, v);
        y = mem;
        t += h;
        printf("%f %f %f\n", t, y, v);
    }
}
double f(double x)
{
    return 0.5 * (1 + x) * (1 + x);
}
double fy(double x, double y, double v)
{
    return v;
}
double fv(double x, double y, double v)
{
    return -1 * (k / m * y + l / m * v);
}