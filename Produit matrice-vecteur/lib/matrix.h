//
// Created by Ayaz BADOURALY on 19/02/16.
//

#ifndef PRODUIT_MATRICE_VECTEUR_MATRIX_H
#define PRODUIT_MATRICE_VECTEUR_MATRIX_H

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

struct Matrix
{
	unsigned int dim_rows;
	unsigned int dim_cols;
	TYPE_ELMT **matrix;
};

struct Matrix construct_matrix ( const unsigned int dim_rows, const unsigned int dim_cols );                    /* create a null matrix */
struct Matrix load_matrix ( const char *filename, const unsigned int first_row, const unsigned int first_col,
                            const unsigned int num_rows, const unsigned int num_cols, MPI_Comm comm );          /* create a matrix from a file ( it can be a sub-matrix of the file ) */
void print_matrix ( const struct Matrix *mtx );                                                                 /* print a matrix in stdout */
void randomize_matrix ( struct Matrix *mtx, const TYPE_ELMT max_value );                                        /* feed a matrix with random values as elements */
int save_matrix ( const struct Matrix *mtx, const char *filename, const char *modes );                          /* save a matrix to a file */
void destruct_matrix ( struct Matrix *mtx );                                                                    /* free memory space */

#endif //PRODUIT_MATRICE_VECTEUR_MATRIX_H
