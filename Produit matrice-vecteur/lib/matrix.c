//
// Created by Ayaz BADOURALY on 19/02/16.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "matrix.h"


struct Matrix construct_matrix ( const unsigned int dim_rows, const unsigned int dim_cols )
{
	struct Matrix mtx;

	mtx.dim_rows = dim_rows;
	mtx.dim_cols = dim_cols;

	mtx.matrix = malloc( mtx.dim_rows * sizeof(TYPE_ELMT *) );
	for ( int i = 0 ; i < mtx.dim_rows ; i++ ) {
		mtx.matrix[i] = calloc( mtx.dim_cols, sizeof(TYPE_ELMT) );
	}

	return mtx;
}

struct Matrix load_matrix ( const char *filename, const unsigned int first_row, const unsigned int first_col, const unsigned int num_rows, const unsigned int num_cols, MPI_Comm comm)
{
	/*
	 * we have to ensure that (first_row , dim_rows) and (first_col , dim_col) are coherent with data !
	 */

	char *line;
	int current_row = 0, current_col,
			offset, step;
	FILE *stream = NULL;
	struct Matrix mtx = {0, 0, NULL};
	TYPE_ELMT elmt;

	mtx = construct_matrix(num_rows, num_cols);

	stream = fopen(filename, "r");
	if ( stream == NULL )
		file_opening_failure(filename, comm);

	while ( current_row < first_row ) {
		free(get_line(stream, filename, comm));
		current_row++;
	}

	for ( ; current_row < first_row + num_rows ; ++current_row )
	{
		line = get_line(stream, filename, comm);
		current_col = 0;
		offset = 0;

		while ( current_col < first_col ) {
#if   TYPE_NUM == 0
			sscanf(line + offset, "%d%n",  &elmt, &step);
#elif TYPE_NUM == 1
			sscanf(line + offset, "%ld%n", &elmt, &step);
#elif TYPE_NUM == 2
			sscanf(line + offset, "%f%n",  &elmt, &step);
#elif TYPE_NUM == 3
			sscanf(line + offset, "%lf%n", &elmt, &step);
#endif
			offset += step;
			current_col++;
		}

		for ( ; current_col < first_col + num_cols ; ++current_col )
		{
#if   TYPE_NUM == 0
			sscanf(line + offset, "%d%n",  &elmt, &step);
#elif TYPE_NUM == 1
			sscanf(line + offset, "%ld%n", &elmt, &step);
#elif TYPE_NUM == 2
			sscanf(line + offset, "%f%n",  &elmt, &step);
#elif TYPE_NUM == 3
			sscanf(line + offset, "%lf%n", &elmt, &step);
#endif
			mtx.matrix[current_row - first_row][current_col - first_col] = elmt;
			offset += step;
		}

		free(line);
	}

	fclose(stream);

	return mtx;
}

void print_matrix ( const struct Matrix *mtx )
{
	printf("[");

	for ( int i = 0 ; i < mtx->dim_rows ; ++i ) {
		for ( int j = 0; j < mtx->dim_cols ; ++j ) {
#if   TYPE_NUM == 0
			printf(" %2d",    mtx->matrix[i][j]);
#elif TYPE_NUM == 1
			printf(" %10ld",  mtx->matrix[i][j]);
#elif TYPE_NUM == 2
			printf(" %.6f",   mtx->matrix[i][j]);
#elif TYPE_NUM == 3
			printf(" %.12lf", mtx->matrix[i][j]);
#endif

			if ( j != mtx->dim_cols - 1 )
				printf(" ;");
		}

		if ( i != mtx->dim_rows - 1) printf("\n ");
		else printf(" ]\n");
	}
}

void randomize_matrix ( struct Matrix *mtx, const TYPE_ELMT max_value )
{
	for ( int i = 0 ; i < mtx->dim_rows ; ++i ) {
		for (int j = 0; j < mtx->dim_cols ; ++j ) {
			mtx->matrix[i][j] = (TYPE_ELMT)rand() / (TYPE_ELMT)(RAND_MAX / max_value);
		}
	}
}

int save_matrix ( const struct Matrix *mtx, const char *filename, const char *modes )
{
	FILE *stream = fopen( filename, modes );
	if ( stream == NULL )
		return EXIT_FAILURE;

	for ( int i = 0 ; i < mtx->dim_rows ; ++i ) {
		for ( int j = 0; j < mtx->dim_cols ; ++j ) {
#if   TYPE_NUM == 0
			fprintf(stream, "%d",  mtx->matrix[i][j]);
#elif TYPE_NUM == 1
			fprintf(stream, "%ld", mtx->matrix[i][j]);
#elif TYPE_NUM == 2
			fprintf(stream, "%f",  mtx->matrix[i][j]);
#elif TYPE_NUM == 3
			fprintf(stream, "%lf", mtx->matrix[i][j]);
#endif

			if ( j != mtx->dim_cols - 1 )
				fprintf(stream, " ");
		}

		fprintf(stream, "\n");
	}

	fclose(stream);

	return EXIT_SUCCESS;
}

void destruct_matrix ( struct Matrix *mtx )
{
	for ( int i = 0 ; i < mtx->dim_rows ; ++i ) {
		free(mtx->matrix[i]);
		mtx->matrix[i] = NULL;
	}

	free(mtx->matrix);
	mtx->matrix = NULL;
}
