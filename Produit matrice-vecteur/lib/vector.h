//
// Created by Ayaz BADOURALY on 21/02/16.
//

#ifndef PRODUIT_MATRICE_VECTEUR_VECTOR_H
#define PRODUIT_MATRICE_VECTEUR_VECTOR_H

#ifndef TYPE_NUM
#define TYPE_NUM 0
#endif

#ifndef TYPE_ELMT
#if TYPE_NUM == 1
#define TYPE_ELMT long
#elif TYPE_NUM == 2
#define TYPE_ELMT float
#elif TYPE_NUM == 3
#define TYPE_ELMT double
#else
#undef TYPE_NUM
#define TYPE_NUM  0
#define TYPE_ELMT int
#endif
#endif

#include <mpi.h>

struct Vector
{
	unsigned int dim;
	TYPE_ELMT *vector;
};

enum Representation
{
	HORIZONTAL, VERTICAL
};

struct Vector construct_vector ( const unsigned int dim );                                                              /* create a null matrix */
struct Vector load_vector ( const char *filename, const unsigned int first_component,
                            const unsigned int num_components, MPI_Comm comm);                                          /* create a vector from a file ( it can be a sub-vector of the file ) */
void print_vector ( const struct Vector *vec, const enum Representation repr );                                         /* print a vector in stdout */
void randomize_vector ( struct Vector *vec, const TYPE_ELMT max_value );                                                /* feed a vector with random values as elements */
int save_vector ( const struct Vector *vec, const enum Representation repr, const char *filename, const char *modes );  /* save a vector to a file */
void destruct_vector ( struct Vector *vec );                                                                            /* free memory space */

#endif //PRODUIT_MATRICE_VECTEUR_VECTOR_H
