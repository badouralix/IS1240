//
// Created by Ayaz BADOURALY
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WITH_LIBPNG
#include <png.h>
#endif

#include "file.h"
#include "matrix.h"


struct COO_Matrix construct_coo_matrix ( const int nnz )
{
	struct COO_Matrix mtx;

	mtx.nnz = nnz;
	mtx.rows = malloc(mtx.nnz * sizeof(int));
	mtx.cols = malloc(mtx.nnz * sizeof(int));
	mtx.values = malloc(mtx.nnz * sizeof(double));

	return mtx;
}

struct COO_Matrix from_graphfile_coo_matrix ( const char *filename, int *nodes_number )
/*
 * we should use hashtable instead of mapping
 */
{
	/*
	 * return the adjacency matrix of the graph in file, which path is filename
	 * return the number of nodes ( size of the matrix ) in nodes_number
	 * return a array of string in mapping, which contains the mapping between row/col of the matrix and node name
	 */

	/***********************************************************/

	bool nodes_number_known = false;
	char *line = NULL, *arrow_from = NULL, *arrow_to = NULL,
			**mapping = NULL;
	FILE *stream = NULL;
	int known_nodes_number = 0, arrow_from_index = 0, arrow_to_index = 0;
	struct COO_Matrix mtx = construct_coo_matrix(0);

	/***********************************************************/

	stream = fopen(filename, "r");
	if ( stream == NULL ) {
		file_opening_failure(filename);
	}

	line = malloc(buffer_size * sizeof(char));
	arrow_from = malloc(buffer_size * sizeof(char));
	arrow_to = malloc(buffer_size * sizeof(char));

	/*
	 * start reading file line by line
	 */
	while ( (line = get_line(stream, filename)) != NULL )
	{
		/*
		 * skiping commented line
		 */
		if ( strncmp(line, "#", 1) == 0 ) {
			continue;
		}

		/*
		 * parsing line
		 * cases are : first line ( number of nodes ) / others lines ( arrows )
		 */
		if ( ! nodes_number_known ) {
			sscanf(line, "%d", nodes_number);
			mapping = malloc(*nodes_number * sizeof(char *));
			nodes_number_known = true;
		}
		else {
			arrow_from_index = -1;
			arrow_to_index = -1;

			sscanf(line, "%s %s", arrow_from, arrow_to);

			/*
			 * get indexes from mapping
			 */
			for ( int i = 0 ; i < known_nodes_number ; ++i ) {
				if ( strcmp(arrow_from, mapping[i]) == 0 ) {
					arrow_from_index = i;
				}
				if ( strcmp(arrow_to, mapping[i]) == 0 ) {
					arrow_to_index = i;
				}

				if ( arrow_from_index != -1 && arrow_to_index != -1 ) {
					break;
				}
			}
			if ( arrow_from_index == -1 ) {
				arrow_from_index = known_nodes_number;
				mapping[arrow_from_index] = malloc(sizeof(arrow_from));
				strcpy(mapping[arrow_from_index], arrow_from);
				++known_nodes_number;
			}
			if ( strcmp(arrow_to, arrow_from) == 0 ) {
				arrow_to_index = arrow_from_index;
			}
			else if ( arrow_to_index == -1 ) {
				arrow_to_index = known_nodes_number;
				mapping[arrow_to_index] = malloc(sizeof(arrow_to));
				strcpy(mapping[arrow_to_index], arrow_to);
				++known_nodes_number;
			}

			/*
			 * adding arrow to the matrix
			 */
			set_value_coo_matrix(&mtx, arrow_to_index, arrow_from_index, 1);
		}
	}

	fclose(stream);
	free(line);
	free(arrow_from);
	free(arrow_to);

	/*
	 * saving mapping table in a file
	 */
	stream = fopen(get_file_path("mapping.txt", OUTPUT), "w");
	if ( stream == NULL ) {
		file_opening_failure(filename);
	}
	for ( int i = 0 ; i < *nodes_number ; ++i ) {
		//printf("index = %d\t->\tnode name = \"%s\"\n", i, mapping[i]);
		fprintf(stream, "index = %d\t->\tnode name = \"%s\"\n", i, mapping[i]);
	}
	fclose(stream);
	free(mapping);

	return mtx;
}

double get_value_coo_matrix ( const struct COO_Matrix *mtx, const int row, const int col )
{
	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		if ( mtx->rows[i] ==  row && mtx->cols[i] == col ) {
			return mtx->values[i];
		}
	}

	return 0;
}

int set_value_coo_matrix ( struct COO_Matrix *mtx, const int row, const int col, const double value )
{
	/*
	 * return EXIT_SUCCESS if value was added to matrix
	 * return EXIT_FAILURE if value is 0 or if an error occured
	 */

	/***********************************************************/

	double *new_values = NULL;
	int i = 0, new_nnz = 0,
			*new_rows = NULL, *new_cols = NULL;

	/***********************************************************/

	if ( value == 0 ) {
		return EXIT_FAILURE;
	}

	new_nnz = mtx->nnz + 1;
	new_rows = malloc(new_nnz * sizeof(int));
	new_cols = malloc(new_nnz * sizeof(int));
	new_values = malloc(new_nnz * sizeof(double));

	if ( new_rows == NULL
			|| new_cols == NULL
			|| new_values == NULL ) {
		free(new_rows);
		free(new_cols);
		free(new_values);

		fprintf(stderr, "\nAn error occurred in set_value_coo_matrix...\n");
		exit(EXIT_FAILURE);
	}

	for ( i = 0 ; i < mtx->nnz ; ++i ) {
		if ( mtx->rows[i] == row && mtx->cols[i] == col ) {
			/*
			 * there already is a value at (row,col) coordinates
			 * we put the new value
			 */
			mtx->values[i] = value;

			free(new_rows);
			free(new_cols);
			free(new_values);

			return EXIT_SUCCESS;
		}
		else if (( mtx->rows[i] == row && mtx->cols[i] > col )
					|| mtx->rows[i] > row ) {
			new_rows[i] = row;
			new_cols[i] = col;
			new_values[i] = value;

			break; /* ending for loop */
		}
		else
		{
			new_rows[i] = mtx->rows[i];
			new_cols[i] = mtx->cols[i];
			new_values[i] = mtx->values[i];
		}
	}

	if ( i == mtx->nnz ) {
		/*
		 * value hasn't been added to new_values
		 * so we append it at the end
		 */
		new_rows[new_nnz - 1] = row;
		new_cols[new_nnz - 1] = col;
		new_values[new_nnz - 1] = value;
	}
	else {
		/*
		 * value has been added at index i
		 * so we append the last indexes of mtx to new_
		 */
		memcpy(&new_rows[i + 1], &mtx->rows[i], (mtx->nnz - i)*sizeof(int));
		memcpy(&new_cols[i + 1], &mtx->cols[i], (mtx->nnz - i)*sizeof(int));
		memcpy(&new_values[i + 1], &mtx->values[i], (mtx->nnz - i)*sizeof(double));
	}

	/*
	 * updating mtx
	 */
	destruct_coo_matrix(mtx);
	mtx->rows = new_rows;
	mtx->cols = new_cols;
	mtx->values = new_values;
	mtx->nnz = new_nnz;

	return EXIT_SUCCESS;
}

int sum_coo_matrix ( struct COO_Matrix *mtx_dest, const struct COO_Matrix *mtx_src )
{
	/*
	 * this function operate : mtx_dest = mtx_dest + mtx_src
	 * return EXIT_FAILURE if an error occured
	 */
	int status;

	for ( int i = 0 ; i < mtx_src->nnz ; ++i )
	{
		status = set_value_coo_matrix(mtx_dest, mtx_src->rows[i], mtx_src->cols[i], mtx_dest->values[i] + mtx_src->values[i] );

		if ( status == EXIT_FAILURE ) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int copy_coo_matrix ( struct COO_Matrix *mtx_dest, const struct COO_Matrix *mtx_src )
{
	/*
	 * this function operate : mtx_dest = mtx_src
	 * return EXIT_FAILURE if an error occured
	 */
	int status;

	for ( int i = 0 ; i < mtx_src->nnz ; ++i )
	{
		status = set_value_coo_matrix(mtx_dest, mtx_src->rows[i], mtx_src->cols[i], mtx_src->values[i] );

		if ( status == EXIT_FAILURE ) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void print_coo_matrix ( const struct COO_Matrix *mtx )
{
	printf("Row indexes : [");
	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		printf(" %d", mtx->rows[i]);
		if ( i != mtx->nnz - 1 ) {
			printf(" ;");
		}
	}
	printf(" ]\n");

	printf("Col indexes : [");
	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		printf(" %d", mtx->cols[i]);
		if ( i != mtx->nnz - 1 ) {
			printf(" ;");
		}
	}
	printf(" ]\n");

	printf("\nVal indexes : [");
	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		printf(" %.6f", mtx->values[i]);
		if ( i != mtx->nnz - 1 ) {
			printf(" ;");
		}
	}
	printf(" ]\n");

	return;
}

int draw_coo_matrix ( const struct COO_Matrix *mtx, const char *filename )
{
	int dim = 0;

	if ( mtx->nnz == 0 ) {
		return EXIT_FAILURE;
	}

	/*
	 * we can find out he dimensions of the square matrix
	 * by calculating max of (mtx->rows,mtx->cols)
	 */
	dim = mtx->rows[mtx->nnz - 1];      /* mtx.rows is ordered */
	for ( int i = 0 ; i < mtx->nnz ; ++i ) {
		dim = mtx->cols[i] > dim ? mtx->cols[i] : dim;
	}
	++dim;


	/************************************************************/


	/*
	 * opening file in binary mode for writing
	 */
	FILE *stream = fopen(filename, "wb");
	if (stream == NULL) {
		file_opening_failure(filename);
	}

	/*
	 * initializing write structure
	 */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Error while allocating write struct\n");
	}

	/*
	 * initializing info structure
	 */
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Error while allocating info struct\n");
	}

	/*
	 * setting error handling
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error while creating png\n");
	}

	png_init_io(png_ptr, stream);

	/*
	 * writing header (1 bit colour depth)
	 */
	png_set_IHDR(png_ptr, info_ptr, dim, dim,
	             8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/*
	 * creating the image row by row
	 */
	png_bytep row = (png_bytep) malloc(dim * sizeof(png_byte));

	for ( int i = 0 ; i < dim ; ++i ) {
		#pragma omp parallel for
		for ( int j = 0 ; j < dim ; ++j ) {
			if ( get_value_coo_matrix(mtx, i, j) ) {
				row[j] = 0xff;
			}
			else {
				row[j] = 0x00;
			}
		}

		if ( verbose ) {
			printf("Writing line %d\n", i);
		}
		png_write_row(png_ptr, row);
	}

	/*
	 * end of writing
	 */
	png_write_end(png_ptr, info_ptr);

	free(row);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(stream);


	return EXIT_SUCCESS;
}

void destruct_coo_matrix ( struct COO_Matrix *mtx )
{
	free(mtx->rows);
	mtx->rows = NULL;

	free(mtx->cols);
	mtx->cols = NULL;

	free(mtx->values);
	mtx->values = NULL;
}
