//
// Created by Ayaz BADOURALY on 21/02/16.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __unix__
#include "../lib/vector.h"
#endif
#ifdef _WIN32
#include "..\lib\vector.h"
#endif


int main ( int argc, char *argv[] )
{
	MPI_Init(&argc, &argv);

	printf("Testing constructor...\n\n");

	struct Vector vec = construct_vector(3);
	printf("Vector vec after initialization :\n");
	print_vector(&vec, HORIZONTAL);
	print_vector(&vec, VERTICAL);

	srand( (unsigned) time(NULL) );
	randomize_vector(&vec, 100);
	printf("\nVector vec after randomization :\n");
	print_vector(&vec, HORIZONTAL);
	print_vector(&vec, VERTICAL);
	//save_vector(&vec, VERICAL, "test", "w");

	destruct_vector(&vec);

	printf("\nTesting loading from file...\n\n");

	vec = load_vector("data/vector", 3, 2, MPI_COMM_WORLD);
	print_vector(&vec, VERTICAL);
	destruct_vector(&vec);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
