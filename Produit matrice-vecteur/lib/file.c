//
// Created by alphabet on 23/02/16.
//

#include <limits.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"


unsigned int buffer_size = BUFSIZ;


char* get_executable_directory ( )
{
	char *resolved = malloc( (PATH_MAX + 1) * sizeof(char) );

	if ( realpath("/proc/self/exe", resolved) != NULL )
	{
		int index_of_last_slash, i = 0;
		while ( resolved[i] != '\0' )
		{
			if ( resolved[i] == '/' )
				index_of_last_slash = i;
			++i;
		}
		resolved[index_of_last_slash + 1] = '\0';

		return resolved;
	}

	free(resolved);
	return NULL;
}

char* get_file_path ( const char *filename )
{
	char *path = malloc( (PATH_MAX + 1) * sizeof(char) ),
			*dir = get_executable_directory();
	bool working_on_nice_linux = dir == NULL ? false : true;

	if ( working_on_nice_linux )
	{
		strcpy(path, dir);
		strcat(path, "../data/");
		strcat(path, filename);

		free(dir);
	}
	else
	{
		strcpy(path, filename);
		strcat(path, ".txt");
	}

	return path;
}

char* get_line ( FILE *stream, const char *filename, MPI_Comm comm)
{
	char *line = malloc(buffer_size * sizeof(char) );

	fgets(line, buffer_size, stream);

	char *eol = strchr(line, '\n');
	if ( eol == NULL )
	{
		fprintf(stderr, "\nAn error occurred when parsing file \"%s\"...\nTry again after increasing buffer size...\n\n", filename);
		MPI_Abort(comm, 2);
	}
	else
	{
		*eol = '\0';
	}

	return line;

}

void file_opening_failure ( const char *filename, MPI_Comm comm )
{
	fprintf(stderr, "\nOpening file \"%s\" failed...\n\n", filename);
	MPI_Abort(comm, 2);
}
