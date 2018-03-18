//
// Created by Ayaz BADOURALY
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "t_matrix.h"
#ifdef __unix__
#include "../lib/file.h"
#include "../lib/matrix.h"
#endif
#ifdef __WIN32
#include "..\lib\file.h"
#include "..\lib\matrix.h"
#endif

bool verbose = true;

int main ( int argc, char *argv[] )
{
	printf("Begin test of matrix vector...\n\n");

	//t_get_value_coo_matrix();
	//t_set_value_coo_matrix();
	//t_from_graphfile_coo_matrix(get_file_path("interstices_5.txt", INPUT));
#ifdef WITH_LIBPNG
	t_draw_coo_matrix();
#endif

	printf("\nEnd test of matrix vector...\n");

	return EXIT_SUCCESS;
}

void t_get_value_coo_matrix ( )
{
	printf("Begin test of get_value_coo_matrix...\n");

	struct COO_Matrix mtx = construct_coo_matrix(0);
	set_value_coo_matrix(&mtx, 1, 2, 5);
	print_coo_matrix(&mtx);
	printf("--\n");

	printf("Value (1,1) is %f\n", get_value_coo_matrix(&mtx, 1, 1));
	printf("Value (1,2) is %f\n", get_value_coo_matrix(&mtx, 1, 2));

	destruct_coo_matrix(&mtx);

	printf("End test of get_value_coo_matrix...\n");
}

void t_set_value_coo_matrix ( )
{
	printf("Begin test of set_value_coo_matrix...\n");

	struct COO_Matrix mtx = construct_coo_matrix(0);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 1, 2, 5);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 2, 2, 1.2);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 0, 2, 0.6);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 2, 1, 3.2);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 2, 3, 3.2);
	print_coo_matrix(&mtx);
	printf("--\n");

	set_value_coo_matrix(&mtx, 2, 3, 2.2);
	print_coo_matrix(&mtx);

	destruct_coo_matrix(&mtx);

	printf("End test of set_value_coo_matrix...\n");
}

void t_from_graphfile_coo_matrix ( char *filename )
{
	printf("Begin test of from_graphfile_coo_matrix...\n");

	int nodes_number;

	struct COO_Matrix mtx = construct_coo_matrix(0);
	print_coo_matrix(&mtx);
	printf("--\n");

	mtx = from_graphfile_coo_matrix(filename, &nodes_number);
	print_coo_matrix(&mtx);
	printf("Number of rows/cols = %d\n", nodes_number);

	/*for ( int i = 0 ; i < nodes_number ; ++i ) {
		printf(mapping[i]);
	}
	free(mapping);*/

	destruct_coo_matrix(&mtx);

	printf("End test of from_graphfile_coo_matrix...\n");
}

#ifdef WITH_LIBPNG
void t_draw_coo_matrix ( )
{
	printf("Begin test of draw_coo_matrix...\n");

	int nodes_number = 0;
	char *filename = "p2p-Gnutella04.txt";
	struct COO_Matrix mtx = construct_coo_matrix(0);
	mtx = from_graphfile_coo_matrix(get_file_path(filename, INPUT), &nodes_number);

	print_coo_matrix(&mtx);

	draw_coo_matrix(&mtx, get_file_path(filename, OUTPUT));

	destruct_coo_matrix(&mtx);

	printf("End test of draw_coo_matrix...\n");
}
#endif
