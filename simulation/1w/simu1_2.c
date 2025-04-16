#include <stdio.h>
#include <math.h>
#define N (16 + 1)
#define MIN 0.0
#define PI 3.14159265358979323846
#define MAX PI / 2
float Simpson(float min, float max, int n);
float f(float x)
{
    return sin(x);
}

int main(void)
{
    float result = Simpson(MIN, MAX, N - 1);
    printf("The result is: %f\n", result);
    printf("The number of intervals is: %d\n", N - 1);
    return 0;
}

float Simpson(float min, float max, int n)
{
    float result = f(min) + f(max);
    for (int i = 1; i < N; i += 2)
    {
        result += 4 * f((max - min) * i / n);
    }
    for (int i = 2; i < N - 1; i += 2)
    {
        result += 2 * f((max - min) * i / n);
    }
    result *= (max - min) / (3 * n);
    return result;
}