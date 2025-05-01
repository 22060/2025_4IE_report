#include <stdio.h>
#include <stdlib.h>
double func(double x, double y)
{
    return 2 * y / (x + 1);
}
double f(double x)
{
    return 0.5 * (1 + x) * (1 + x);
}
int main(void)
{
    double x = 0.0;
    double y = 0.5;
    double h = 0.5;
    double k1 = 0.0;
    double k2 = 0.0;
    double k3 = 0.0;
    double k4 = 0.0;

    for (int i = 1; i < 5; i++)
    {
        k1 = h * func(x, y);
        k2 = h * func(x + h / 2, y + k1 / 2);
        k3 = h * func(x + h / 2, y + k2 / 2);
        k4 = h * func(x + h, y + k3);
        // printf("k1 = %f, k2 = %f, k3 = %f, k4 = %f\n", k1, k2, k3, k4);
        y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
        x += h;
        printf("x_%d = %f, y_%d = %f, y_true = %f,error = %f\n", i, x, i, y, f(x), f(x) - y);
        // Update x for the next iteration
    }
}