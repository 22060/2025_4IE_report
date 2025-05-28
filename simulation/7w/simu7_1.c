#include <stdio.h>
#include <string.h>
#include <stdlib.h>
double hI(double t, double I, double Q);
double hq(double t, double I, double Q);
double r = 1.0;
double c = 0.3;
double l = 10.0;
double q0 = 10;
double h = 0.1;
int main(void)
{
    double k1, k2, l1, l2;
    double t = 0, I = 0.0, Q = q0;
    printf("# t,I,Q\n");
    while (t < 10.0)
    {
        printf("%f %f %f\n", t, I, Q);
        k1 = h * hI(t, I, Q);
        l1 = h * hq(t, I, Q);
        k2 = h * hI(t + h, I + k1, Q + l1);
        l2 = h * hq(t + h, I + k1, Q + l1);
        I += (k1 + k2) / 2;
        Q += (l1 + l2) / 2;
        t += h;
    }
}
double hI(double t, double I, double Q)
{
    return -1 * (r * I / l + Q / l / c);
}
double hq(double t, double I, double Q)
{
    return I;
}