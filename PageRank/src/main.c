//
// Created by Ayaz BADOURALY
//

#include <limits.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include <getopt.h>
#endif

#include "main.h"
#include "pagerank.h"
#ifdef __unix__
#include "../lib/file.h"
#endif
#ifdef _WIN32
#include "..\lib\file.h"
#endif

bool verbose = false;
int root = 0;

int main ( int argc, char *argv[] )
{
	/***********************************************
	 *            Initiating MPI session           *
	 ***********************************************/

	int rank, size;

	MPI_Init (&argc, &argv);	            /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

	root = size - 1;

	/*
     * deal with incoherent parameters
     */
	if ( rank == root && size < 2 ) {
		fprintf(stderr, "\nNumber of processes must be greater than 2.\n\n");
		MPI_Abort(MPI_COMM_WORLD, 3);
	}


	/***********************************************
	 *              Setting environment            *
	 ***********************************************/

	double precision = 1e-6;
	char input_filename[PATH_MAX] = "", *eol = NULL;
#ifdef __unix__
	char *eof = NULL;   // useful variable to convert string to number
	extern char *optarg;
	int param = 0;
#endif

	/*
	 * parsing passed arguments
	 */
#ifdef __unix__
	while ( (param = getopt(argc, argv, "b:i:p:v")) != -1 )
		switch ( param )
		{
			case 'b':
				buffer_size = strtol(optarg, &eof, 10);
				break;
			case 'i':
				strcpy(input_filename, optarg);
				break;
			case 'p':
				precision = strtod(optarg, &eof);
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				break;
		}
#endif

	/*
	 * if input_filename was not passed as an argument
	 * ask for an input filename
	 */
	if ( strcmp(input_filename, "") == 0 && rank == 0 ) {
		printf("Please give an input filename : ");
		fflush(stdout);

		fgets(input_filename, sizeof(input_filename), stdin);
		eol = strchr(input_filename, '\n');
		if ( eol == NULL ) {
			fprintf(stderr, "\nAn error occurred when parsing your input...\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		else {
			*eol = '\0';
		}
	}

	MPI_Bcast(input_filename, PATH_MAX, MPI_CHAR, 0, MPI_COMM_WORLD);

	/*
	 * summarize parameters
	 */
	if ( verbose && rank == root ) {
		printf("Input filename : %s\n", input_filename);
		printf("Precision : %.12f\n", precision);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	/***********************************************
     *                   Saving                    *
     ***********************************************/

	FILE *stream;

	if ( rank == root ) {
		char *settings_file = get_file_path("settings.txt", OUTPUT);
		stream = fopen( settings_file, "w" );
		if ( stream == NULL ) {
			file_opening_failure(settings_file);
			return EXIT_FAILURE;
		}

		fprintf(stream, "Input filename : %s\n", input_filename);
		fprintf(stream, "Precision : %.12f\n", precision);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	/***********************************************
     * Everything is fine, we can start processing *
     ***********************************************/

	if ( compute_pagerank ( get_file_path(input_filename, INPUT), precision ) == EXIT_SUCCESS && rank == root ) {
		printf("PageRank was successfully calculated !\n");
		fprintf(stream, "PageRank was successfully calculated !\n");
		fclose(stream);
	}
	else if ( rank == root ) {
		fprintf(stderr, "PageRank did not converge...\n");
		fprintf(stream, "PageRank did not converge...\n");
		fclose(stream);
	}

	/***********************************************
     *           Terminating MPI session           *
     ***********************************************/

	MPI_Finalize();
	return EXIT_SUCCESS;
}

static void usage ( const char *cmd )
{
	fprintf(stderr, "Usage: %s [-b buffer_size] [-i input_filename] [-p precision] [-h] [-v]\n\n", cmd);
	MPI_Abort(MPI_COMM_WORLD, 1);
}
