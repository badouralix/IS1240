//
// Created by Ayaz BADOURALY on 22/02/16.
//

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __unix__
#include <getopt.h>
#endif

#include "calculator.h"

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
#ifdef __unix__
	bool verbose = false;
	char *eof = NULL;   // useful variable to convert string to number
	extern char *optarg;
	int param;
#endif

	/*
	 * setting buffer size
	 * ( using BUFSIZ if no argument passed )
	 */
#ifdef __unix__
	while ( (param = getopt(argc, argv, "b:v")) != -1 )
		switch ( param )
		{
			case 'b':
				buffer_size = strtol(optarg, &eof, 10);
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				break;
		}
#endif


	/***********************************************
     * Everything is fine, we can start processing *
     ***********************************************/


	/***********************************************
	 *     Getting settings from metadata file     *
	 ***********************************************/

	char *metadata_filename = get_file_path("metadata"),
			*line = NULL, *matrix_filename = NULL, *vector_filename = NULL;
	FILE *metadata_stream = NULL;
	int dim_rows, dim_cols,
			num_rows = 0, first_row = 0;

	metadata_stream = fopen(metadata_filename, "r");
	if ( metadata_stream == NULL )
		file_opening_failure(metadata_filename, MPI_COMM_WORLD);

	if ( rank == root )
	{
		printf("Reading config from file \"%s\"\n", metadata_filename);

		line = get_line(metadata_stream, metadata_filename, MPI_COMM_WORLD);
		sscanf(line, "%d %d", &dim_rows, &dim_cols);
		free(line);

		printf("Dimension of the problem is set to %d.\n", dim_cols);
		if ( verbose ) printf("\n");
	}
	else
	{
		/* this works if not running on a cluster */
		/*free(get_line(metadata_stream, metadata_filename, MPI_COMM_WORLD));
		matrix_filename = get_line(metadata_stream, metadata_filename, MPI_COMM_WORLD);
		vector_filename = get_line(metadata_stream, metadata_filename, MPI_COMM_WORLD);*/

		/* this works even on for a cluster distributed computation */
		matrix_filename = get_file_path("matrix");
		vector_filename = get_file_path("vector");
	}

	fclose(metadata_stream);

	/*
     * deal with incoherent parameters
     */
	if ( rank == root && (size < 2 || size > dim_rows + 1) )
	{
		fprintf(stderr, "\nNumber of processes must be between 2 and %d ( dimension of the problem and a root process ).\n\n", dim_rows + 1);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}


	/***********************************************
	 *    Sending parameters to every processes    *
	 ***********************************************/

	/*
	 * theses vertex contain indexes of offsets for the MPI_Allgatherv
	 */
	int recv_counts[size], displs[size];

	if ( rank == root )
	{
		for ( int p = 0 ; p < root ; ++p )
		{
			num_rows = p < dim_rows % (size-1) ? dim_rows/(size-1) + 1 : dim_rows/(size-1);
			MPI_Send(&num_rows, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
			recv_counts[p] = num_rows;

			MPI_Send(&first_row, 1, MPI_INT, p, 1, MPI_COMM_WORLD);
			displs[p] = first_row;
			first_row += num_rows;
		}

		recv_counts[root] = 0;
		displs[root] = first_row;
	}
	else
	{
		MPI_Recv(&num_rows, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&first_row, 1, MPI_INT, root, 1, MPI_COMM_WORLD, &status);

		if ( verbose )
			printf("Process %d will work with num_rows=%d with first_row=%d\n", rank, num_rows, first_row);
	}

	/***********************************************/

	//MPI_Bcast(&dim_rows, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&dim_cols, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(recv_counts, size, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(displs, size, MPI_INT, root, MPI_COMM_WORLD);


	/***********************************************
	 *             Starting computing              *
	 ***********************************************/

	struct Matrix local_mtx = {0, 0, NULL};
	struct Vector local_vec = {0, NULL}, local_result = {0, NULL},
			global_vec = construct_vector(dim_cols);

	if ( rank != root )
	{
		local_mtx = load_matrix(matrix_filename, first_row, 0, num_rows, dim_cols, MPI_COMM_WORLD);
		local_vec = load_vector(vector_filename, first_row, num_rows, MPI_COMM_WORLD);
		local_result = construct_vector(num_rows);
	}

#if   TYPE_NUM == 0
	MPI_Allgatherv( local_vec.vector, recv_counts[rank], MPI_INT,    global_vec.vector, recv_counts, displs, MPI_INT,    MPI_COMM_WORLD );
#elif TYPE_NUM == 1
	MPI_Allgatherv( local_vec.vector, recv_counts[rank], MPI_LONG,   global_vec.vector, recv_counts, displs, MPI_LONG,   MPI_COMM_WORLD );
#elif TYPE_NUM == 2
	MPI_Allgatherv( local_vec.vector, recv_counts[rank], MPI_FLOAT,  global_vec.vector, recv_counts, displs, MPI_FLOAT,  MPI_COMM_WORLD );
#elif TYPE_NUM == 3
	MPI_Allgatherv( local_vec.vector, recv_counts[rank], MPI_DOUBLE, global_vec.vector, recv_counts, displs, MPI_DOUBLE, MPI_COMM_WORLD );
#endif

	if ( rank != root )
	{
		for ( int i = 0 ; i < num_rows ; ++i ) {
			for (int j = 0; j < dim_cols; ++j)
				local_result.vector[i] += local_mtx.matrix[i][j] * global_vec.vector[j];

			if ( verbose )
				num_rows == 1 ? printf("p%d : progression = 100%%\n", rank) : printf("p%d : progression = %d%%\n", rank, (i * 100)/(num_rows - 1));
		}

		destruct_matrix(&local_mtx);
		destruct_vector(&global_vec);
	}

	free(matrix_filename);
	free(vector_filename);

	MPI_Barrier(MPI_COMM_WORLD);


	/***********************************************
	 *           Saving data into files            *
	 ***********************************************/

	bool ping = false, pong = false;
	char *result_filename= get_file_path("result");

	if ( rank == root )
	{
		remove(result_filename);

		ping = true;
		for ( int p = 0 ; p < root ; ++p )
		{
			pong = false;
			MPI_Send(&ping, 1, MPI_C_BOOL, p, 2, MPI_COMM_WORLD);
			MPI_Recv(&pong, 1, MPI_C_BOOL, p, 2, MPI_COMM_WORLD, &status);

			if ( ! pong )
			{
				fprintf(stderr, "\nSomething wrong occurred when trying to save local sub-vector of process %d to file...\n\n", p);
				MPI_Abort(MPI_COMM_WORLD, 2);
			}
		}
	}
	else
	{
		MPI_Recv(&ping, 1, MPI_C_BOOL, root, 2, MPI_COMM_WORLD, &status);

		if ( ping )
		{
			if ( save_vector(&local_result, VERTICAL, result_filename, "a") == EXIT_FAILURE )
				file_opening_failure(result_filename, MPI_COMM_WORLD);

			pong = true;
			MPI_Send(&pong, 1, MPI_C_BOOL, root, 2, MPI_COMM_WORLD);
		}
		else
		{
			fprintf(stderr, "\nSomething wrong occurred when trying to save local sub-vector to file...\n\n");
			MPI_Abort(MPI_COMM_WORLD, 2);
		}
	}

	free(result_filename);


	/***********************************************
	 *    Merging local results on root process    *
	 ***********************************************/

	if ( verbose )
	{
		struct Vector global_result = construct_vector(dim_cols);

#if   TYPE_NUM == 0
		MPI_Gatherv( local_result.vector, recv_counts[rank], MPI_INT,    global_result.vector, recv_counts, displs, MPI_INT,    root, MPI_COMM_WORLD );
#elif TYPE_NUM == 1
		MPI_Gatherv( local_result.vector, recv_counts[rank], MPI_LONG,   global_result.vector, recv_counts, displs, MPI_LONG,   root, MPI_COMM_WORLD );
#elif TYPE_NUM == 2
		MPI_Gatherv( local_result.vector, recv_counts[rank], MPI_FLOAT,  global_result.vector, recv_counts, displs, MPI_FLOAT,  root, MPI_COMM_WORLD );
#elif TYPE_NUM == 3
		MPI_Gatherv( local_result.vector, recv_counts[rank], MPI_DOUBLE, global_result.vector, recv_counts, displs, MPI_DOUBLE, root, MPI_COMM_WORLD );
#endif

		if ( rank == root )
		{
			printf("\nResult of the computation is : ");
			print_vector(&global_result, HORIZONTAL);
		}

		destruct_vector(&global_result);
	}

	destruct_vector(&local_result);


	/***********************************************
     *           Terminating MPI session           *
     ***********************************************/

	MPI_Finalize();
	return EXIT_SUCCESS;
}

static void usage ( const char *cmd )
{
	fprintf(stderr, "Usage: %s [-b buffer_size] [-h] [-v]\n\n", cmd);
	MPI_Abort(MPI_COMM_WORLD, 1);
}
