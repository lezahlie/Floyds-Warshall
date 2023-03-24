/**
 * @file print-graph.c
 * @author Leslie Horace
 * @brief Main program to read a matrix from and file print it's contents to the console
 * @version 1.0
 * 
 */
#include "utilities.h"

int main(int argn, char **argv) {
    int ret = 1; 
    // check if all args were entered 
    if (argn != 2) {       
        printf("Usage: %s <input_data_file>\n", argv[0]);
        goto end;
    }   
    // read in matrix from (.dat) file
    GraphShared gs = {NULL, 0};
    char * infile = argv[1];
    printf("Reading adjacency matrix from '%s'\n", infile);
    if((ret = read_graph(&gs.A, &gs.n, infile)) != 0) goto end; 
     // print the matrix 
    print_graph(gs); 
    free(gs.A);   
end:
    exit(ret); 
}
