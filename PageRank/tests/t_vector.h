//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_T_VECTOR_H
#define PAGERANK_T_VECTOR_H

#ifdef __unix__
#include "../lib/vector.h"
#endif
#ifdef _WIN32
#include "..\lib\vector.h"
#endif

void randomize_vector ( struct Vector *vec, const double max_value );

void t_construct_vector ( );
void t_unit_vector ( );
void t_get_value_vector ( );
void t_set_value_vector ( );
void t_distance_vector ( );
void t_save_vector ( );

#endif //PAGERANK_T_VECTOR_H
