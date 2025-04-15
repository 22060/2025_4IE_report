#include <stdio.h>
#define N 9
#define MIN 0.0
#define MAX 1.0
float y[N] = {4.0, 3.938, 3.765, 3.507, 3.200, 2.876, 2.560, 2.265, 2.000};
int main(void)
{
    float result = y[0] + y[N - 1];
    printf("The result is: %f\n", result);
    for (int i = 1; i < N; i += 2)
    {
        result += 4 * y[i];
    }
    for (int i = 2; i < N - 1; i += 2)
    {
        result += 2 * y[i];
    }
    result *= (MAX - MIN) / (3 * (N - 1));
    printf("The result is: %f\n", result);
    return 0;
}