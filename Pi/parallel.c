#include <stdio.h>
#include <mpi.h>


double f( double x)
{
    /* calcul de f(x) */
    return 4/(1+x*x);
}


double sequential_computation (double a, double b, double h, int n)
{
    double x = a;

    double sub_integral = (f(a) + f(b))/2;
    for ( int i = 1 ; i < n ; i++ )
    {
        x += h;
        sub_integral += f(x);
    }
    sub_integral *= h;

    return sub_integral;
}

int main (int argc, char *argv[])
{
	double a = 0, b = 1;     // points gauche et droits
	int n = 100000;         // nombre de trapèzes, divisible par size !!!
	double h = (b-a)/n;      // pas

	int rank, size;
	MPI_Init (&argc, &argv);	            /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	if ( rank == 0 )
	{
		double integral = 0, sub_integral;

		printf("Calculation from process %d\n", rank);
		double local_a = a + rank * (b - a) / size,
				local_b = a + ( rank + 1 ) * (b - a) / size;
		integral = sequential_computation(local_a, local_b, h, n/size);
		printf("Estimation d'ordre 0 : %f\n", integral);

		for (int i = 1 ; i < size ; i++ )
		{
			printf("Waiting for process %d to compute...\n", i);
			MPI_Recv(&sub_integral, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			integral += sub_integral;
			printf("Estimation d'ordre %d : %f\n", i, integral);
		}

		printf("Estimation : %f\n", integral);
	}
	else
	{
		printf("Calculation from process %d\n", rank);
		double local_a = a + rank * (b - a) / size,
				local_b = a + ( rank + 1 ) * (b - a) / size;

		double sub_integral = sequential_computation(local_a, local_b, h, n/size);
		MPI_Send(&sub_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

    MPI_Finalize();

    return 0;
}