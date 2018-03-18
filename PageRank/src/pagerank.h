//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_PAGERANK_H
#define PAGERANK_PAGERANK_H

#include <stdbool.h>

#ifdef __unix__
#include "../lib/matrix.h"
#include "../lib/vector.h"
#endif
#ifdef _WIN32
#include "..\lib\matrix.h"
#include "..\lib\vector.h"
#endif

extern bool verbose;
extern int root;

int compute_pagerank ( const char *filename, const double precision );                          /* compute the pagerank of the graph in filename */
int load_graph ( struct Matrix *adj_mtx, const char *filename );                                /* load graph in filename in an adjacency matrix */
void normalize ( struct Matrix *mtx, const int dim );                                           /* transform matrix into a semi-stochastic matrix */
void distribute_matrix ( struct Matrix *mtx, const int nodes_numnber );                         /* send pieces of the matrix to non-root process */
void set_damping_factor ( struct Matrix *mtx );                                                 /* multiply matrix by DAMPING_FACTOR */
struct Vector multiply_matrix_vector ( const struct Matrix *mtx, const struct Vector *vec );    /* multiple a vector by a coo matrix */
void add_random_move ( struct Vector *vec, const int dim );                                     /* modelize the suffer getting bored and starting on another random page */
int lauch_computing ( struct Matrix *mtx, const int dim, const double precision );              /* loop to compute the pagerank */


#endif //PAGERANK_PAGERANK_H
