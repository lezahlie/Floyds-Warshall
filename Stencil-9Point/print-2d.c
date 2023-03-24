/**
 * @file print-2d.c
 * @author Leslie Horace
 * @brief Main program to read a matrix from and file print it's contents to the console
 * @version 2.0
 * 
 */
    
#include "utilities.h"

int main(int argn, char **argv) {
    int ret = 1; 
    // check if all args were entered 
    if (argn != 2) {       
        printf("Usage: %s <input_data_file>\n", argv[0]);
    }else{
        char * infile = argv[1];
        double *A;
        int n = 0, m = 0;
        // read in matrix from (.dat) file
        printf("Reading data from '%s'", infile);
        if((ret = read2D(&A, &n, &m, infile)) == 0){
            print2D(A, n, m);       // print the matrix
            free(A);
        }
    }
    exit(ret); 
}
