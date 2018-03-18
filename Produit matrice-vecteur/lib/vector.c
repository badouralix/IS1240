//
// Created by Ayaz BADOURALY on 21/02/16.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "vector.h"


struct Vector construct_vector ( const unsigned int dim )
{
	struct Vector vec;

	vec.dim = dim;

	vec.vector = calloc( vec.dim, sizeof(TYPE_ELMT) );

	return vec;
}

struct Vector load_vector ( const char *filename, const unsigned int first_component, const unsigned int num_components, MPI_Comm comm)
{
	/*
	 * WARNING ! currently, vector has to be recorded vertically in the file, not horizontally !
	 * we have to ensure that (first_row , dim_rows) is coherent with data !
	 */

	char *line;
	int current_component = 0;
	FILE *stream = NULL;
	struct Vector vec = {0, NULL};
	TYPE_ELMT elmt;

	vec = construct_vector(num_components);

	stream = fopen(filename, "r");
	if ( stream == NULL )
		file_opening_failure(filename, comm);

	while ( current_component < first_component ) {
		free(get_line(stream, filename, comm));
		current_component++;
	}

	for ( ; current_component < first_component + num_components ; ++current_component )
	{
		line = get_line(stream, filename, comm);

#if   TYPE_NUM == 0
		sscanf(line, "%d",  &elmt);
#elif TYPE_NUM == 1
		sscanf(line, "%ld", &elmt);
#elif TYPE_NUM == 2
		sscanf(line, "%f",  &elmt);
#elif TYPE_NUM == 3
		sscanf(line, "%lf", &elmt);
#endif
		vec.vector[current_component - first_component] = elmt;

		free(line);
	}

	fclose(stream);

	return vec;
}

void print_vector ( const struct Vector *vec, const enum Representation repr )
{
	printf("[");

	for ( int i = 0 ; i < vec->dim ; ++i ) {
#if   TYPE_NUM == 0
		printf(" %2d",    vec->vector[i]);
#elif TYPE_NUM == 1
		printf(" %10ld",  vec->vector[i]);
#elif TYPE_NUM == 2
		printf(" %.6f",   vec->vector[i]);
#elif TYPE_NUM == 3
		printf(" %.12lf", vec->vector[i]);
#endif

		if ( i != vec->dim - 1 ) {
			if ( repr == HORIZONTAL )
				printf(" ;");
			else if ( repr == VERTICAL )
				printf("\n ");
		}
	}

	printf(" ]\n");
}

void randomize_vector ( struct Vector *vec, const TYPE_ELMT max_value )
{
	for (int i = 0; i < vec->dim; ++i) {
		vec->vector[i] = (TYPE_ELMT) rand() / (TYPE_ELMT) (RAND_MAX / max_value);
	}
}

int save_vector ( const struct Vector *vec, const enum Representation repr, const char *filename, const char *modes )
{
	FILE *stream = fopen( filename, modes );
	if ( stream == NULL )
		return EXIT_FAILURE;

	for ( int i = 0 ; i < vec->dim ; ++i ) {
#if   TYPE_NUM == 0
		fprintf(stream, "%d",  vec->vector[i]);
#elif TYPE_NUM == 1
		fprintf(stream, "%ld", vec->vector[i]);
#elif TYPE_NUM == 2
		fprintf(stream, "%f",  vec->vector[i]);
#elif TYPE_NUM == 3
		fprintf(stream, "%lf", vec->vector[i]);
#endif

		if ( i != vec->dim - 1 ) {
			if ( repr == HORIZONTAL )
				fprintf(stream, " ");
			else if ( repr == VERTICAL )
				fprintf(stream, "\n");
		}
	}
	fprintf(stream, "\n");

	fclose(stream);

	return EXIT_SUCCESS;
}

void destruct_vector ( struct Vector *vec )
{
	free(vec->vector);
	vec->vector = NULL;
}
