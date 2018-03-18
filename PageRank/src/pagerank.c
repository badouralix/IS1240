//
// Created by Ayaz BADOURALY
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "pagerank.h"
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

#ifdef WITH_CUDA
#endif

#define MAX_ITERATION 1000
#define DAMPING_FACTOR 0.85

int compute_pagerank ( const char *filename, const double precision )
/*
 * WARNING : filename contain the real path to the file
 * therefore, there is no need to call get_file_path
 */
{
	int rank, size, output,
			nodes_number = 0;
	struct Matrix mtx = construct_coo_matrix(0);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	/***********************************************
     *         Reading the adjacency matrix        *
     ***********************************************/

	nodes_number = load_graph(&mtx, filename);

	distribute_matrix(&mtx, nodes_number);

	/***********************************************
     *        Pre-compute the damping factor       *
     *          and the holes in the graph         *
     ***********************************************/

	if ( rank != root ) {
		set_damping_factor(&mtx);
	}

	/***********************************************
     *              Compute the pagerank           *
     ***********************************************/

	output = lauch_computing(&mtx, nodes_number, precision);

	destruct_coo_matrix(&mtx);

	return output;
}

int load_graph ( struct Matrix *adj_mtx, const char *filename )
/*
 * return the number of nodes of the graph, ie the dimension of the adjacency matrix
 */
{
	int rank, size,
			nodes_number = 0;
	struct Matrix local_mtx = construct_coo_matrix(0);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	/***********************************************
     *           Reading adjacency matrix          *
     ***********************************************/

	if ( rank == root ) {
		local_mtx = from_graphfile_coo_matrix(filename, &nodes_number);
		copy_coo_matrix(adj_mtx, &local_mtx);

		/*
         * deal with incoherent parameters
         */
		if (size > nodes_number + 1) {
			fprintf(stderr,
			        "\nNumber of processes must be less than %d ( dimension of the problem and a root process ).\n\n",
			        nodes_number + 1);
			MPI_Abort(MPI_COMM_WORLD, 3);
		}

		/*
		 * calculating the normalisation of the adjacency matrix
		 */
		normalize(adj_mtx, nodes_number);
	}

	destruct_coo_matrix(&local_mtx);

	MPI_Bcast(&nodes_number, 1, MPI_INT, root, MPI_COMM_WORLD);
	return nodes_number;
}

void distribute_matrix ( struct Matrix *mtx, const int nodes_number )
{
	int rank, size,
			num_rows = 0, index_first_row = 0, index_last_row = -1, nnz = 0;;
	MPI_Status status;
	struct Matrix local_mtx = construct_coo_matrix(0);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	if ( rank == root ) {
		for ( int p = 0 ; p < root ; ++p ) {
			/*
			 * for each process p :
			 * root knows the index ( COO matrix format ) of the first row sent to p
			 * root calculates the number of rows of the sub-matrix sent to p
			 * root calculates the index of the last row sent to p
			 * root deduces the number of non-zero elements of the sub-matrix sent to p
			 * root sends the sub-matrix
			 */

			num_rows = p < nodes_number % (size-1) ? nodes_number/(size-1) + 1 : nodes_number/(size-1);

			for ( int i = index_first_row ; i < mtx->nnz ; ++i ) {
				if ( mtx->rows[i] >= mtx->rows[index_first_row] + num_rows ) {
					break;
				}
				++index_last_row;
			}

			nnz = index_last_row - index_first_row + 1;

			MPI_Send(&nnz, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
			if ( nnz != 0 ) {
				MPI_Send((*mtx).rows + index_first_row, nnz, MPI_INT, p, 1, MPI_COMM_WORLD);
				MPI_Send((*mtx).cols + index_first_row, nnz, MPI_INT, p, 2, MPI_COMM_WORLD);
				MPI_Send((*mtx).values + index_first_row, nnz, MPI_DOUBLE, p, 3, MPI_COMM_WORLD);
			}

			index_first_row = index_last_row + 1;
		}
	}
	else {
		MPI_Recv(&(local_mtx.nnz), 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);

		if ( local_mtx.nnz != 0 ) {
			local_mtx = construct_coo_matrix(local_mtx.nnz);
			MPI_Recv(local_mtx.rows, local_mtx.nnz, MPI_INT, root, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(local_mtx.cols, local_mtx.nnz, MPI_INT, root, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(local_mtx.values, local_mtx.nnz, MPI_DOUBLE, root, 3, MPI_COMM_WORLD, &status);
		}

		copy_coo_matrix(mtx, &local_mtx);
	}

	destruct_coo_matrix(&local_mtx);

	return;
}

void normalize ( struct Matrix *mtx, const int dim )
/*
 * transform the matrix into a stochastic matrix ( vertically )
 * except if column is null
 */
{
	double *col_sum = calloc(dim, sizeof(double));    /* contains the sum of each column */

	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		col_sum[mtx->cols[i]] += mtx->values[i];
	}

	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		mtx->values[i] /= col_sum[mtx->cols[i]];
	}

	free(col_sum);

	return;
}

void set_damping_factor ( struct Matrix *mtx )
/*
 * multiple the matrix by the dumping factor
 */
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	if ( rank == root ) {
		printf("Warning : function set_damping_factor should not be called by root process...\n");
	}

	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		set_value_coo_matrix(mtx, mtx->rows[i], mtx->cols[i], DAMPING_FACTOR * mtx->values[i]);
	}

	return;
}

struct Vector multiply_matrix_vector ( const struct Matrix *mtx, const struct Vector *vec )
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	struct Vector result = construct_vector(vec->dim);  /* assuming dimension of vec is the dimension of the problem */

	if ( rank == root ) {
		printf("Warning : function multiply_matrix_vector should not be called by root process...\n");
	}

	for (int i = 0; i < mtx->nnz; ++i) {
		set_value_vector(&result, mtx->rows[i], get_value_vector(&result, mtx->rows[i])
		                                        + get_value_coo_matrix(mtx, mtx->rows[i], mtx->cols[i]) * get_value_vector(vec, mtx->cols[i]));
	}
	//printf("rank %d : ", rank);
	//print_vector(&result, HORIZONTAL);

	return result;
}

void add_random_move ( struct Vector *vec, const int dim )
/*
 * add (1-d)/N where d is the DAMPING_FACTOR and N is dim, the dimension of the matrix
 */
{
	int rank;
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	double random_move = (double)(1 - DAMPING_FACTOR) / (double)dim;

	for ( int i = 0 ; i < vec->dim ; ++i ) {
		set_value_vector(vec, i, get_value_vector(vec, i)
		                         + random_move );
	}

	return;
}

int lauch_computing ( struct Matrix *mtx, const int dim, const double precision )
{
	int rank, i, output = 4;
	double distance = 0;
	struct Vector pagerank = unit_vector(dim),
			sub_pagerank = construct_vector(0), next_pagerank = construct_vector(dim);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */

	for ( i = 0 ; i < MAX_ITERATION ; ++i ) {
		if ( rank != root ) {
			sub_pagerank = multiply_matrix_vector(mtx, &pagerank);
		}

		MPI_Allreduce(sub_pagerank.vector, next_pagerank.vector, dim, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		add_random_move(&next_pagerank, dim);

		if ( rank == root )
		{
			distance = distance_vector(&pagerank, &next_pagerank);

			if ( verbose ) {
				printf("iteration %d ( delta = %f ) : ", i, distance);
				print_vector(&pagerank, HORIZONTAL);
			}
		}

		MPI_Bcast(&distance, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);

		if ( distance < precision ) {
			output = EXIT_SUCCESS;
			break;
		}

		if ( copy_vector(&pagerank, &next_pagerank) == EXIT_FAILURE ) {
			exit(EXIT_FAILURE);
		}
	}

	if ( rank == root ) {
		printf("Loop existed after %d iterations\n", i);
		save_vector(&pagerank, VERTICAL, get_file_path("pagerank.txt", OUTPUT),"w");
	}

	destruct_vector(&pagerank);
	destruct_vector(&sub_pagerank);
	destruct_vector(&next_pagerank);

	return output;
}
