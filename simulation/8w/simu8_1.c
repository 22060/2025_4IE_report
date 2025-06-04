#include <stdio.h>
#include <string.h>
#include <stdlib.h>
double fkx(double t, double x, double y, double vx, double vy);
double fky(double t, double x, double y, double vx, double vy);
double flx(double t, double x, double y, double vx, double vy);
double fly(double t, double x, double y, double vx, double vy);
double h = 0.1;
double m = 1.0; // 質量
double Q = 1.0; // 電荷1
double q = 1.0; // 電荷2
int main(void)
{
    double k1x, k2x, l1x, l2x;
    double k1y, k2y, l1y, l2y;
    double x, y, vx, vy;
    double t = 0.0;
    x = 1.0;  // 初期位置x
    y = 0.0;  // 初期位置y
    vx = 0.0; // 初期速度vx
    vy = 1.0; // 初期速度vy
    printf("# t,x,y\n");
    while (t < 10.0)
    {
        printf("%f %f %f\n", t, x, y);
        k1x = h * fkx(t, x, y, vx, vy);
        k1y = h * fky(t, x, y, vx, vy);
        l1x = h * flx(t, x, y, vx, vy);
        l1y = h * fly(t, x, y, vx, vy);
        k2x = h * fkx(t + h, x + l1x, y + l1y, vx + k1x, vy + k1y);
        k2y = h * fky(t + h, x + l1x, y + l1y, vx + k1x, vy + k1y);
        l2x = h * flx(t + h, x + l1x, y + l1y, vx + k1x, vy + k1y);
        l2y = h * fly(t + h, x + l1x, y + l1y, vx + k1x, vy + k1y);
        vx += (k1x + k2x) / 2;
        vy += (k1y + k2y) / 2;
        x += (l1x + l2x) / 2;
        y += (l1y + l2y) / 2;
        t += h;
        if (t > 10.0)
        {
            break; // 時間が10秒を超えたら終了
        }
    }
}
double fkx(double t, double x, double y, double vx, double vy)
{
    return -1.0 / m * Q * q / (x * x + y * y) * x / sqrt(x * x + y * y);
}
double fky(double t, double x, double y, double vx, double vy)
{
    return -1.0 / m * Q * q / (x * x + y * y) * y / sqrt(x * x + y * y);
}
double flx(double t, double x, double y, double vx, double vy)
{
    return vx;
}
double fly(double t, double x, double y, double vx, double vy)
{
    return vy;
}