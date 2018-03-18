//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_VECTOR_H
#define PAGERANK_VECTOR_H

struct Vector
{
	int dim;
	double *vector;
};

enum Representation
{
	HORIZONTAL, VERTICAL
};

struct Vector construct_vector ( const int dim );                                                                       /* create a null matrix */
struct Vector unit_vector ( const int dim );                                                                            /* create a unit vector whom components are only 1/dimension */

double get_value_vector ( const struct Vector *vec, const int ind );                                                    /* get value of the ind-th component of the vector */
double distance_vector ( const struct Vector *vec1, const struct Vector *vec2 );                                        /* calculate the distance between two vectors */
int copy_vector ( struct Vector *vec_dest, const struct Vector *vec_src );                                              /* copy vector from source to destination */

int set_value_vector ( struct Vector *vec, const int ind, const double value );                                         /* set value to the vector */

void print_vector ( const struct Vector *vec, const enum Representation repr );                                         /* print a vector in stdout */
int save_vector ( const struct Vector *vec, const enum Representation repr, const char *filename, const char *modes );  /* save a vector to a file */

void destruct_vector ( struct Vector *vec );                                                                            /* free memory space */

#endif //PAGERANK_VECTOR_H
