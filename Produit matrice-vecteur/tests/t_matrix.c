//
// Created by Ayaz BADOURALY on 20/02/16.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __unix__
#include "../lib/matrix.h"
#endif
#ifdef _WIN32
#include "..\lib\matrix.h"
#endif


int main ( int argc, char *argv[] )
{
	MPI_Init(&argc, &argv);

	printf("Testing constructor...\n\n");

	struct Matrix mtx = construct_matrix(5,5);
	printf("Matrix mtx after initialization :\n");
	print_matrix(&mtx);

	srand( (unsigned) time(NULL) );
	randomize_matrix(&mtx, 100);
	printf("\nMatrix mtx after randomization :\n");
	print_matrix(&mtx);
	//save_matrix(&mtx, "test", "w");

	destruct_matrix(&mtx);

	printf("\nTesting loading from file...\n\n");

	mtx = load_matrix("data/matrix", 4, 0, 2, 10, MPI_COMM_WORLD );
	print_matrix(&mtx);
	destruct_matrix(&mtx);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
