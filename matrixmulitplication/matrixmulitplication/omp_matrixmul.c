#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main()
{
    int a[10][10], b[10][10], mul[10][10], r, c, i, j, k;
    printf("Enter the number of row = ");
    scanf("%d", &r);
    printf("Enter the number of column = ");
    scanf("%d", &c);

    printf("Enter first matrix:\n");
    for (i = 0; i < r; i++)
    {
        for (j = 0; j < c; j++)
        {
            scanf("%d", &a[i][j]);
        }
    }

    printf("Enter second matrix:\n");
    for (i = 0; i < r; i++)
    {
        for (j = 0; j < c; j++)
        {
            scanf("%d", &b[i][j]);
        }
    }
    struct timeval t0, t1;
    gettimeofday(&t0, 0);

    printf("Product of first and second matrix: \n");

    #pragma omp parallel for
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            for (int k = 0; k < c; k++)
            {
                mul[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    for (i = 0; i < r; i++)
    {
        for (j = 0; j < c; j++)
        {
            printf("%d\t", mul[i][j]);
        }
        printf("\n");
    }
    gettimeofday(&t1, 0);
    double elapsed = (t1.tv_sec - t0.tv_sec) * 1.0f + (t1.tv_usec - t0.tv_usec) / 1000000.0f;
    printf("\ntime required to run %lf", elapsed);
    return 0;
}