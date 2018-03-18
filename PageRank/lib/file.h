//
// Created by Ayaz BADOURALY
//

#ifndef PAGERANK_FILE_H
#define PAGERANK_FILE_H

#include <stdio.h>

enum Location
{
	INPUT, OUTPUT
};

char* get_executable_directory ( );                                     /* return the location of the executable */
char* get_file_path ( const char *filename, const enum Location loc );  /* return a string containing the path of a data file according to its filename */
char* get_line ( FILE *stream, const char *filename );                  /* useful to read a file line by line */
void file_opening_failure ( const char *filename );                     /* abort task if opening file failed */

#endif //PAGERANK_FILE_H
