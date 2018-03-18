//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_MATRIX_H
#define PAGERANK_MATRIX_H

#define Matrix COO_Matrix

extern int buffer_size;
extern bool verbose;

struct COO_Matrix
{
	int nnz;
	int *rows;
	int *cols;
	double *values;
};

struct COO_Matrix construct_coo_matrix ( const int nnz );                                                       /* create a null matrix with COO format */
struct COO_Matrix from_graphfile_coo_matrix ( const char *filename, int *nodes_number );                        /* create a null matrix with COO format from a graph written in a file */

double get_value_coo_matrix ( const struct COO_Matrix *mtx, const int row, const int col );                     /* get value of an element of the coo matrix */

int set_value_coo_matrix ( struct COO_Matrix *mtx, const int row, const int col, const double value );          /* add single element to an existing matrix */
int sum_coo_matrix ( struct COO_Matrix *mtx_dest, const struct COO_Matrix *mtx_src );                           /* sum two matrices */
int copy_coo_matrix ( struct COO_Matrix *mtx_dest, const struct COO_Matrix *mtx_src );                          /* copy matrix from source to destination */

void print_coo_matrix ( const struct COO_Matrix *mtx );                                                         /* print a COO matrix in stdout */
#ifdef WITH_LIBPNG
int draw_coo_matrix ( const struct COO_Matrix *mtx, const char *filename );                                     /* create a picture of a COO matrix */
#endif

void destruct_coo_matrix ( struct COO_Matrix *mtx );                                                            /* free memory space */

#endif //PAGERANK_MATRIX_H
