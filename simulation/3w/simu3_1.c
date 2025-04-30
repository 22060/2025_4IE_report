#include <stdio.h>
double hf(double x, double y)
{
    return 2 * y / (x + 1);
}
double f(double x)
{
    return 0.5 * (1 + x) * (1 + x);
}
int main(void)
{
    double h = 0.5;
    double x = 0.0;
    double y = 0.5;
    double k1 = 0.0;
    double k2 = 0.0;
    while (1)
    {
        k1 = hf(x, y) * h;
        k2 = hf(x + h, y + k1) * h;
        y = y + (k1 + k2) / 2;
        x = x + h;
        printf("tx = %f, ty = %f,y = %f,erroe = %f\n", x, f(x), y, f(x) - y);

        if (x >= 2.0)
        {
            break;
        }
    }
    return 0;
}