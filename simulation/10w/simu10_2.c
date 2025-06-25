
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N 4
void Gauss(double buf[N][N + 1])
{
    printf("入力行列:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N + 1; j++)
        {
            printf("%3.3f\t", buf[i][j]);
        }
        printf("\n");
    }
    printf("前進消去:\n");
    double zyousuu = 0.0;
    int t = 0;
    while (t != N - 1)
    {
        for (int j = 1 + t; j < N; j++)
        {
            zyousuu = buf[j][t] / buf[t][t];
            for (int i = 0; i < N + 1; i++)
            {
                buf[j][i] = buf[j][i] - (buf[t][i] * zyousuu);
            }
        }
        t++;
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N + 1; j++)
        {
            printf("%3.3f\t", buf[i][j]);
        }
        printf("\n");
    }
    double x[N];
    for (int i = N - 1; i >= 0; i--)
    {
        x[i] = buf[i][N];
        for (int j = i + 1; j < N; j++)
        {
            x[i] -= buf[i][j] * x[j];
        }
        x[i] /= buf[i][i];
    }
    printf("解:\n");
    for (int i = 0; i < N; i++)
    {
        printf("x[%d] = %f\n", i, x[i]);
    }
}
int main(void)
{
    double buf[N][N + 1] = {
        {1.0, 2.0, 1.0, 5.0, 20.5},
        {8.0, 1.0, 3.0, 1.0, 14.5},
        {1.0, 7.0, 1.0, 1.0, 18.5},
        {1.0, 1.0, 6.0, 1.0, 9.0}};
    Gauss(buf);
}