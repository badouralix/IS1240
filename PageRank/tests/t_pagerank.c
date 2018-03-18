//
// Created by Ayaz BADOURALY
//

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "t_pagerank.h"
#ifdef __unix__
#include "../lib/file.h"
#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "../src/pagerank.h"
#endif
#ifdef __WIN32
#include "..\lib\file.h"
#include "..\lib\matrix.h"
#include "..\lib\vector.h"
#include "..\src\pagerank.h"
#endif

bool verbose = true;
int root = 0;

int main ( int argc, char *argv[] )
{

	/***********************************************
	 *            Initiating MPI session           *
	 ***********************************************/

	int rank, size;

	MPI_Init (&argc, &argv);	            /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	root = size - 1;

	if ( rank == root && size < 2 ) {
		fprintf(stderr, "\nNumber of processes must be over 2.\n\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}


	/***********************************************
     *                   Testing                   *
     ***********************************************/

	if ( rank == root ) {
		printf("Begin test of pagerank...\n\n");
	}

	//t_compute_pagerank();
	//t_load_graph();
	//t_normalize();
	//t_distribute_matrix();
	//t_set_damping_factor();
	//t_multiply_matrix_vector();
	//t_add_random_move();
	t_lauch_computing();

	if ( rank == root ) {
		printf("\nEnd test of pagerank...\n");
	}


	/***********************************************
     *           Terminating MPI session           *
     ***********************************************/

	MPI_Finalize();
	return EXIT_SUCCESS;
}

void t_compute_pagerank ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of compute_pagerank...\n");
	}

	double precision = 1e-6;
	char *input_filename = "interstices_5.txt";

	compute_pagerank(get_file_path(input_filename, INPUT), precision);

	MPI_Barrier(MPI_COMM_WORLD);

	if ( rank == root ) {
		printf("End test of compute_pagerank...\n");
	}
}

void t_load_graph ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of load_graph...\n");
	}

	char *input_filename = "interstices_3.txt";
	struct Matrix mtx = construct_coo_matrix(0);
	load_graph(&mtx, get_file_path(input_filename, INPUT));

	if ( rank == root ) {
		print_coo_matrix(&mtx);
	}

	if ( rank == root ) {
		printf("End test of load_graph...\n");
	}

	destruct_coo_matrix(&mtx);
}

void t_normalize ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of normalize...\n");

		struct Matrix mtx = construct_coo_matrix(0);

		set_value_coo_matrix(&mtx, 1, 2, 5);
		set_value_coo_matrix(&mtx, 2, 2, 1.2);
		set_value_coo_matrix(&mtx, 0, 2, 0.6);
		set_value_coo_matrix(&mtx, 2, 1, 3.2);
		set_value_coo_matrix(&mtx, 2, 3, 3.2);
		set_value_coo_matrix(&mtx, 2, 3, 2.2);
		print_coo_matrix(&mtx);

		printf("--\n");

		normalize(&mtx, 4);
		print_coo_matrix(&mtx);

		destruct_coo_matrix(&mtx);

		printf("End test of normalize...\n");
	}
}

void t_distribute_matrix ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of distribute_matrix...\n");
	}

	char *input_filename = "interstices_3.txt";
	struct Matrix mtx = construct_coo_matrix(0);
	int nodes_number = load_graph(&mtx, get_file_path(input_filename, INPUT));

	if ( rank == root ) {
		print_coo_matrix(&mtx);
	}

	distribute_matrix(&mtx, nodes_number);

	if ( rank == 0 ) {
		print_coo_matrix(&mtx);
	}

	if ( rank == root ) {
		printf("End test of distribute_matrix...\n");
	}
}

void t_set_damping_factor ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of normalize...\n");

		char *input_filename = "interstices_3.txt";
		struct Matrix mtx = construct_coo_matrix(0);
		load_graph(&mtx, get_file_path(input_filename, INPUT));
		print_coo_matrix(&mtx);

		printf("--\n");

		set_damping_factor(&mtx);
		print_coo_matrix(&mtx);

		destruct_coo_matrix(&mtx);

		printf("End test of normalize...\n");
	}
}

void t_multiply_matrix_vector ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of multiply_matrix_vector...\n");
	}

	char *input_filename = "interstices_5.txt";
	//char *input_filename = "p2p-Gnutella04.txt";
	struct Matrix mtx = construct_coo_matrix(0);
	int dim = load_graph(&mtx, get_file_path(input_filename, INPUT));

	if ( rank == root ) {
		printf("dimension = %d\n", dim);
		print_coo_matrix(&mtx);
	}

	struct Vector vec = unit_vector(dim),
			local_result = rank != root ? multiply_matrix_vector(&mtx, &vec) : construct_vector(dim),
			result = construct_vector(dim);

	destruct_coo_matrix(&mtx);

	if ( rank == root ) {
		printf("vec = ");
		print_vector(&vec, HORIZONTAL);
	}
	else {
		printf("local_result = ");
		print_vector(&local_result, HORIZONTAL);
	}

	MPI_Allreduce(local_result.vector, result.vector, vec.dim, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	destruct_vector(&vec);
	destruct_vector(&local_result);

	if ( rank == root ) {
		printf("result = ");
		print_vector(&result, HORIZONTAL);
	}

	destruct_vector(&result);

	if ( rank == root ) {
		printf("End test of multiply_matrix_vector...\n");
	}
}

void t_add_random_move ( )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of add_random_move...\n");

		struct Vector vec = construct_vector(5);
		printf("vec = ");
		print_vector(&vec, HORIZONTAL);

		printf("--\n");

		add_random_move(&vec, 5);
		printf("vec = ");
		print_vector(&vec, HORIZONTAL);

		printf("End test of add_random_move...\n");
	}
}

void t_lauch_computing ( )
{
	int rank, output;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Begin test of lauch_computing...\n");
	}

	char *filename = "interstices_5.txt";

	struct Matrix mtx = construct_coo_matrix(0);
	int nodes_number = load_graph(&mtx, get_file_path(filename, INPUT));
	distribute_matrix(&mtx, nodes_number);

	if ( rank == root ) {
		printf("Graph is : \n");
		print_coo_matrix(&mtx);
	}

	if ( rank != root ) {
		set_damping_factor(&mtx);
	}

	output = lauch_computing(&mtx, nodes_number, 1e-6);
	if ( rank == root ) {
		printf("output = %d\n", output);
	}

	destruct_coo_matrix(&mtx);

	if ( rank == root ) {
		printf("End test of lauch_computing...\n");
	}
}
