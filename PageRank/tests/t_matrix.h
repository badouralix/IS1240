//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_T_MATRIX_H
#define PAGERANK_T_MATRIX_H

void t_get_value_coo_matrix ( );
void t_set_value_coo_matrix ( );
void t_from_graphfile_coo_matrix ( char *filename );
#ifdef WITH_LIBPNG
void t_draw_coo_matrix ( );
#endif

#endif //PAGERANK_T_MATRIX_H
