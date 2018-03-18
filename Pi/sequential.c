#include <stdio.h>
#include <mpi.h>


float f( float x)
{
    /* calcul de f(x) */
    return 4/(1+x*x);
}


int main (int argc, char *argv[])
{
    float integral;         // résultat
    float a = 0, b = 1;     // points gauche et droits
    int n = 10000;          // nombre de trapèzes
    float h = (b-a)/n;      // pas
    float x = a;

    integral = (f(a) + f(b))/2;
    for ( int i = 1 ; i < n ; i++ )
    {
        x += h;
        integral += f(x);
    }
    integral *= h;

    printf("Estimation : %f\n", integral);

    return 0;
}