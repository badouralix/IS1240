//
// Created by Ayaz BADOURALY
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "vector.h"


struct Vector construct_vector ( const int dim )
{
	struct Vector vec;

	vec.dim = dim;

	vec.vector = calloc( vec.dim, sizeof(double) );

	return vec;
}

struct Vector unit_vector ( const int dim )
{
	struct Vector vec;

	vec.dim = dim;

	vec.vector = malloc( vec.dim * sizeof(double) );
	for ( int i = 0 ; i < dim ; ++i ) {
		vec.vector[i] = (double) 1 / dim;
	}

	return vec;
}

double get_value_vector ( const struct Vector *vec, const int ind )
{
	if ( ind >= 0 && ind < vec->dim ) {
		return vec->vector[ind];
	}

	fprintf(stderr, "\nAn error occurred in get_value_vector...\n");
	exit(EXIT_FAILURE);
}

double distance_vector ( const struct Vector *vec1, const struct Vector *vec2 )
{
	/*
	 * as vectors are in finite dimension, all norms are equivalent
	 * this function use infinity norm
	 */
	if ( vec1->dim != vec2->dim ) {
		return -1;
	}

	double distance = 0;

	for ( int i = 0 ; i < vec1->dim ; ++i ) {
		distance = fmax(distance, fabs(vec1->vector[i] - vec2->vector[i]));
	}

	return distance;
}

int copy_vector ( struct Vector *vec_dest, const struct Vector *vec_src )
{
	/*
	 * this function operate : vec_dest = vec_src
	 * return EXIT_FAILURE if an error occured
	 */
	int status;

	for ( int i = 0 ; i < vec_src->dim ; ++i )
	{
		status = set_value_vector(vec_dest, i, vec_src->vector[i] );

		if ( status == EXIT_FAILURE ) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int set_value_vector ( struct Vector *vec, const int ind, const double value )
{
	if ( ind >= 0 && ind < vec->dim ) {
		vec->vector[ind] = value;
		return EXIT_SUCCESS;
	}

	fprintf(stderr, "\nAn error occurred in set_value_vector...\n");
	exit(EXIT_FAILURE);
}

void print_vector ( const struct Vector *vec, const enum Representation repr )
{
	printf("[");

	for ( int i = 0 ; i < vec->dim ; ++i ) {
		printf(" %.6f", vec->vector[i]);

		if ( i != vec->dim - 1 ) {
			if ( repr == HORIZONTAL )
				printf(" ;");
			else if ( repr == VERTICAL )
				printf("\n ");
		}
	}

	printf(" ]\n");
}

int save_vector ( const struct Vector *vec, const enum Representation repr, const char *filename, const char *modes )
{
	FILE *stream = fopen( filename, modes );
	if ( stream == NULL ) {
		file_opening_failure(filename);
		return EXIT_FAILURE;
	}



	for ( int i = 0 ; i < vec->dim ; ++i ) {
		fprintf(stream, "%.12f", vec->vector[i]);

		if ( i != vec->dim - 1 ) {
			if ( repr == HORIZONTAL ) {
				fprintf(stream, " ");
			}
			else if ( repr == VERTICAL ) {
				fprintf(stream, "\n");
			}
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
