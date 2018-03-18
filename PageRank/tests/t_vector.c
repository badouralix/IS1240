//
// Created by Ayaz BADOURALY
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "t_vector.h"
#ifdef __unix__
#include "../lib/file.h"
#include "../lib/vector.h"
#endif
#ifdef _WIN32
#include "..\lib\file.h"
#include "..\lib\vector.h"
#endif


int main ( int argc, char *argv[] )
{
	printf("Begin test of lib vector...\n\n");
	srand( (unsigned) time(NULL) );

	//t_construct_vector();
	t_unit_vector();
	//t_get_value_vector();
	//t_set_value_vector();
	//t_distance_vector();
	//t_save_vector();

	printf("\nEnd test of lib vector...\n");

	return EXIT_SUCCESS;
}

void randomize_vector ( struct Vector *vec, const double max_value )
{
	for (int i = 0; i < vec->dim; ++i) {
		vec->vector[i] = (double) rand() / (double) (RAND_MAX / max_value);
	}
}

void t_construct_vector( )
{
	printf("Begin test of construct_vector...\n");

	struct Vector vec = construct_vector(3);
	printf("Vector vec after initialization :\n");
	print_vector(&vec, HORIZONTAL);
	print_vector(&vec, VERTICAL);

	destruct_vector(&vec);

	printf("End test of construct_vector...\n");
}

void t_unit_vector ( )
{
	printf("Begin test of unit_vector...\n");

	struct Vector vec = unit_vector(5);
	print_vector(&vec, HORIZONTAL);

	destruct_vector(&vec);

	printf("End test of unit_vector...\n");
}

void t_get_value_vector ( )
{
	printf("Begin test of get_value_vector...\n");

	struct Vector vec = construct_vector(5);
	randomize_vector(&vec, 1);
	print_vector(&vec, HORIZONTAL);

	printf("vec[%d] is %f\n", 3, get_value_vector(&vec, 3));

	destruct_vector(&vec);

	printf("End test of get_value_vector...\n");
}

void t_set_value_vector ( )
{
	printf("Begin test of set_value_vector...\n");

	struct Vector vec = construct_vector(5);
	print_vector(&vec, HORIZONTAL);

	printf("Setting component %d to %f\n", 3, 1.2);
	set_value_vector(&vec, 3, 1.2);
	print_vector(&vec, HORIZONTAL);

	destruct_vector(&vec);

	printf("End test of set_value_vector...\n");
}

void t_distance_vector ( )
{
	printf("Begin test of distance_vector...\n");

	struct Vector vec1 = construct_vector(3),
			vec2 = construct_vector(3);

	randomize_vector(&vec1, 1);
	randomize_vector(&vec2, 1);

	printf("vec1 : ");
	print_vector(&vec1, HORIZONTAL);
	printf("vec2 : ");
	print_vector(&vec2, HORIZONTAL);

	printf("Distance between vec1 and vec2 = %f\n", distance_vector(&vec1, &vec2));

	printf("End test of distance_vector...\n");
}

void t_save_vector ( )
{
	printf("Begin test of save_vector...\n");

	struct Vector vec = construct_vector(5);
	randomize_vector(&vec, 1);
	print_vector(&vec, HORIZONTAL);
	save_vector(&vec, VERTICAL, get_file_path("t_vector.txt", OUTPUT), "w");

	printf("End test of save_vector...\n");
}
