//
// Created by Ayaz BADOURALY
//

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

int buffer_size = BUFSIZ;

char* get_executable_directory ( )
{
	char *resolved = malloc( (PATH_MAX + 1) * sizeof(char) );

	if ( realpath("/proc/self/exe", resolved) != NULL ) {
		int index_of_last_slash, i = 0;

		while ( resolved[i] != '\0' ) {
			if ( resolved[i] == '/' ) {
				index_of_last_slash = i;
			}
			++i;
		}
		resolved[index_of_last_slash + 1] = '\0';

		return resolved;
	}

	free(resolved);
	return NULL;
}

char* get_file_path ( const char *filename, const enum Location loc )
{
	char *path = malloc( (PATH_MAX + 1) * sizeof(char) ),
			*dir = get_executable_directory();
	bool working_on_nice_linux = dir == NULL ? false : true;

	if ( working_on_nice_linux ) {
		strcpy(path, dir);
		if ( loc == INPUT ) {
			strcat(path, "../input/");
		}
		else if ( loc == OUTPUT ) {
			strcat(path, "../output/");
		}
		strcat(path, filename);

		free(dir);
	}
	else {
		printf("Warning : assuming file %s is in the current directory...", filename);
		strcpy(path, filename);
	}

	return path;
}

char* get_line ( FILE *stream, const char *filename )
{
	char *line = malloc(buffer_size * sizeof(char) );

	if ( fgets(line, buffer_size, stream) == NULL ) {
		return NULL;
	}

	char *eol = strchr(line, '\n');
	if ( eol == NULL ) {
		fprintf(stderr, "\nAn error occurred when parsing file \"%s\"...\nTry again after increasing buffer size...\n\n", filename);
		exit(2);
	}
	else {
		*eol = '\0';
	}

	return line;
}

void file_opening_failure ( const char *filename )
{
	fprintf(stderr, "\nOpening file \"%s\" failed...\n\n", filename);
	exit(2);
}
