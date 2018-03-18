#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX_N 10    // size of vectors
#define MAX_X 100    // max value of vector's components

void print_vector( int vec[MAX_N] );
void randomize_vector( int vec[MAX_N] );

int main (int argc, char *argv[])
{
	const int root = 0;
	int rank, size;
	MPI_Init (&argc, &argv);	            /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	int a[MAX_N], b[MAX_N],
		res, local_res = 0;

	if ( rank == root )
	{
		srand(time(NULL));

		randomize_vector(a);
		print_vector(a);

		randomize_vector(b);
		print_vector(b);
	}

	MPI_Bcast(a, MAX_N, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(b, MAX_N, MPI_INT, root, MPI_COMM_WORLD);

	for ( int i = rank ; i < MAX_N ; i+= size )
		local_res += a[i] * b[i];

	MPI_Reduce(&local_res, &res, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

	if ( rank == root )
		printf("Scalar product = %d\n", res);

	MPI_Finalize();

	return 0;
}

void print_vector( int vec[MAX_N] )
{
	printf("[ ");

	for ( int i = 0 ; i < MAX_N - 1 ; i++ )
		printf(" %2d ;", vec[i]);

	printf(" %2d ]\n", vec[MAX_N-1]);
}

void randomize_vector( int vec[MAX_N] )
{
	for ( int i = 0 ; i < MAX_N ; i++ )
	{
		vec[i] = rand() % MAX_X;
	}
}
