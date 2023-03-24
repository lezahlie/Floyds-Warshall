/**
 * @file make-2d.c
 * @author Leslie Horace
 * @brief Main program to create a matrix and write it to a file
 * @version 1.0
 * 
 */
 
#include "utilities.h"

int main(int argn, char **argv) {
    int ret = 1; 
    // check if all args were entered 
    if (argn != 4) { 
        printf("Usage: %s <num_rows> <num_cols> <output data file>\n", argv[0]);
    }else{
        char * nptr,* mptr, * initfile = argv[3];
        // parse <num_rows> <num_cols> as base 10 int
        int n = strtol((char *)argv[1], &nptr, 10); 
        int m = strtol((char *)argv[2], &mptr, 10); 
        // check if <num_rows> <num_cols> are valid
        if((*nptr != '\0' ||  n < 3) || (*mptr != '\0' || m < 3)) {
            printf("Error: <rows><cols> both must be at least 3 (for stencil operations)\n"); 
        }else{
            double *A;
            long mat_size = (long)n*(long)m*sizeof(double);
            if(malloc1D(&A, n, m, "A")==0){ // allocate space for A
                init2D(A, n, m);        // initialize A 
                ret = write2D(A, n, m, initfile);    // write A to output file
                mat_size +=(2*sizeof(int));
                printf("Wrote matrix A[%dx%d] to [%s] --- filesize = %ld(B) = %.6Lg(GB)\n", n, m, initfile, mat_size, BtoGB(mat_size));
                free(A);
            }
        }
    }
    exit(ret);
}
