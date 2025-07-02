
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N 3
void Gauss(double buf[N][N + 1], int pivot)
{
    printf("pivot選択: %s\n", pivot == 1 ? "有効" : "無効");
    double buffer;
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
    int flag = 0;
    while (t != N - 1)
    {
        for (int j = 1 + t; j < N - flag; j++)
        {
            zyousuu = buf[j][t] / buf[t][t];
            for (int i = 0; i < N + 1; i++)
            {
                buf[j][i] = buf[j][i] - (buf[t][i] * zyousuu);
            }
        }
        printf("\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N + 1; j++)
            {
                printf("%3.3f\t", buf[i][j]);
            }
            printf("\n");
        }
        // printf("%f\n", buf[t + 1][t + 1]);
        if (((buf[t + 1][t + 1] >= -0.001 && buf[t + 1][t + 1] <= 0.001) && t + 1 < N - 1) && pivot)
        {
            flag = 1;
            printf("0のため入れ替え→\n");
            for (int k = 0; k < N + 1; k++)
            {
                buffer = buf[t + 1][k];
                buf[t + 1][k] = buf[t + N - 1][k];
                buf[t + N - 1][k] = buffer;
            }
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N + 1; j++)
                {
                    printf("%3.3f\t", buf[i][j]);
                }
                printf("\n");
            }
        }
        else
        {
            flag = 0;
        }
        t++;
    }
    printf("\n");
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
    double zyousuu = 0.0;
    double buf[N][N + 1] = {
        {-2.0, 2.0, 0.0, 0.0},
        {3.0, -3.0, 1.0, 1.0},
        {2.0, 1.0, 6.0, 9.0}};
    Gauss(buf, 1);
}