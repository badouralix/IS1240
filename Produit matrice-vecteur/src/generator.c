//
// Created by Ayaz BADOURALY on 19/02/16.
//

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __unix__
#include <getopt.h>
#endif

#include "generator.h"

#ifdef __unix__
#include "../lib/file.h"
#include "../lib/matrix.h"
#include "../lib/vector.h"
#endif
#ifdef _WIN32
#include "..\lib\file.h"
#include "..\lib\matrix.h"
#include "..\lib\vector.h"
#endif

#define DEFAULT_DIM 10      // default dimension of the generated matrix
#define DEFAULT_MAX 10      // default max value of the elements of the matrix


int main ( int argc, char *argv[] )
{
	/***********************************************
	 *            Initiating MPI session           *
	 ***********************************************/

	int rank, size;
	MPI_Status status;

	MPI_Init (&argc, &argv);	            /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */


	/***********************************************
	 *              Setting environment            *
	 ***********************************************/

	const int root = size - 1;
	int dim = DEFAULT_DIM, max_value = DEFAULT_MAX;
#ifdef __unix__
	char *eof = NULL;       // useful variable to convert string to number
	extern char *optarg;
	int param;
#endif

	/*
	 * setting dimension of the problem
	 * ( using DEFAULT_DIM and DEFAULT_MAX if no argument passed )
	 */
#ifdef __unix__
	while ( (param = getopt(argc, argv, "hn:x:")) != -1 )
		switch ( param )
		{
			case 'n':
				dim = strtol(optarg, &eof, 10);
				break;
			case 'x':
				max_value = strtol(optarg, &eof, 10);
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				break;
		}
#endif

	/*
	 * summerize parameters
	 */
	if ( rank == root )
	{
		printf("Dimension of the problem is set to %d.\n", dim);        // future version will deal with non squared matrix
		printf("Elements won't be greater than %d.\n\n", max_value);
	}

	/*
	 * deal with incoherent parameters
	 */
	if ( rank == root && (size < 2 || size > dim + 1) )
	{
		fprintf(stderr, "\nNumber of processes must be between 2 and %d ( dimension of the problem and a root process ).\n\n", dim + 1);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}


	/***********************************************
	 * Everything is fine, we can start processing *
	 ***********************************************/

	int num_rows = 0;
	struct Matrix sub_mtx = {0, 0, NULL};
	struct Vector sub_vec = {0, NULL};

	/*
	 * setting seed for randomization
	 */
	srand( (unsigned) (time(NULL) * (rank+size)) );

	/*
	 * creating local random sub-matrix and random sub-vector
	 */
	if ( rank == root )
	{
		for ( int p = 0 ; p < root ; ++p )
		{
			num_rows = p < dim % (size-1) ? dim/(size-1) + 1 : dim/(size-1);
			MPI_Send(&num_rows, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&num_rows, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);

		sub_mtx = construct_matrix(num_rows, dim);
		randomize_matrix(&sub_mtx, max_value);

		sub_vec = construct_vector(num_rows);
		randomize_vector(&sub_vec, max_value);
	}


	/***********************************************
	 *           Saving data into files            *
	 ***********************************************/

	bool ping = false, pong = false;
	char *metadata_filename = get_file_path("metadata"),
			*matrix_filename = get_file_path("matrix"),
			*vector_filename = get_file_path("vector");
	FILE *metadata_stream = NULL;

	if ( rank == root )
	{
		metadata_stream = fopen(metadata_filename, "w");
		if ( metadata_stream == NULL )
			file_opening_failure(metadata_filename, MPI_COMM_WORLD);
		fprintf(metadata_stream, "%d %d\n%s\n%s\n", dim, dim, matrix_filename, vector_filename);
		fclose(metadata_stream);

		remove(matrix_filename);
		remove(vector_filename);

		ping = true;
		for ( int p = 0 ; p < root ; ++p )
		{
			pong = false;
			MPI_Send(&ping, 1, MPI_C_BOOL, p, 1, MPI_COMM_WORLD);
			MPI_Recv(&pong, 1, MPI_C_BOOL, p, 1, MPI_COMM_WORLD, &status);

			if ( ! pong )
			{
				fprintf(stderr, "\nSomething wrong occurred when trying to save local sub-matrix and sub-vector of process %d to file...\n\n", p);
				MPI_Abort(MPI_COMM_WORLD, 2);
			}
		}
	}
	else
	{
		MPI_Recv(&ping, 1, MPI_C_BOOL, root, 1, MPI_COMM_WORLD, &status);

		if ( ping )
		{
			if ( save_matrix(&sub_mtx, matrix_filename, "a") == EXIT_FAILURE )
				file_opening_failure(matrix_filename, MPI_COMM_WORLD);
			destruct_matrix(&sub_mtx);

			if ( save_vector(&sub_vec, VERTICAL, vector_filename, "a") == EXIT_FAILURE )
				file_opening_failure(vector_filename, MPI_COMM_WORLD);
			destruct_vector(&sub_vec);

			pong = true;
			MPI_Send(&pong, 1, MPI_C_BOOL, root, 1, MPI_COMM_WORLD);
		}
		else
		{
			fprintf(stderr, "\nSomething wrong occurred when trying to save local sub-matrix and sub-vector to file...\n\n");
			MPI_Abort(MPI_COMM_WORLD, 2);
		}
	}

	free(metadata_filename);
	free(matrix_filename);
	free(vector_filename);

	printf("Process %d is done !\n", rank);


	/***********************************************
     *           Terminating MPI session           *
     ***********************************************/

	MPI_Finalize();
	return EXIT_SUCCESS;
}

static void usage ( const char *cmd )
{
	fprintf(stderr, "Usage: %s [-h] [-n dim] [-x max_value]\n\n", cmd);
	MPI_Abort(MPI_COMM_WORLD, 1);
}
