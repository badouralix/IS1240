#include <stdio.h>
#include <mpi.h>


int main (int argc, char *argv[])
{
	int rank, size,
		val;

	MPI_Init (&argc, &argv);	/* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	if ( rank == 0 )
	{
		printf("Process %d initializing val...\n", rank);
		val = 0;
		printf("Initialization done\nStarting computation...\n");
		MPI_Send(&val, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
	}
	else
	{
		printf("Initializing receiver %d...\n", rank);
		MPI_Recv(&val, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("%d received by %d !\n", val, rank);

		val++;

		if ( rank != size - 1 )
		{
			printf( "%d now sending new val...\n", rank);
			MPI_Send(&val, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
		}
		else
		{
			printf("Processes are all done !\n");
		}

	}

	MPI_Finalize();
	return 0;
}
