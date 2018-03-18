//
// Created by alphabet on 23/02/16.
//

#ifndef PRODUIT_MATRICE_VECTEUR_FILE_H
#define PRODUIT_MATRICE_VECTEUR_FILE_H

#include <mpi.h>
#include <stdio.h>

char* get_executable_directory ( );                                     /* return the location of the executable */
char* get_file_path ( const char *filename );                           /* return a string containing the path of a data file according to its filename */
char* get_line ( FILE *stream, const char *filename, MPI_Comm comm );   /* useful to read a file line by line */
void file_opening_failure ( const char *filename, MPI_Comm comm );      /* abort task if opening file failed */

#endif //PRODUIT_MATRICE_VECTEUR_FILE_H
