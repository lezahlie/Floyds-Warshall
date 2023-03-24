/**
 * @file make-graph.c
 * @author Leslie Horace
 * @brief Main program to create random adjacency matrix and write to a file
 * @version 1.0
 * @note Expected result of A[36] after running floyds.c and/or pth-floyds.c
 *     A[36] = { 0, 2, 5, 3, 6, 9,
 *                   -1, 0, 6, 1, 4, 7,
 *                   -1, 15, 0, 4, 7, 10,
 *                   -1, 11, 5, 0, 3, 6,
 *                   -1, 8, 2, 5, 0, 3,
 *                   -1, 5, 6, 2, 4, 0 };
 *  
*/
#include "utilities.h"

int main(int argn, char **argv) {
    int ret = 1, n = 6;
    // check if all args were entered 
    if (argn != 2) { 
        printf("Usage: %s <file_name>\n", argv[0]);
        goto end;
    }
    char *outfile = argv[1];
    int A[36] = {0, 2, 5, -1, -1, -1,    
                        -1, 0, 7, 1, -1, 8,
                        -1, -1, 0, 4, -1, -1,
                        -1, -1, -1, 0, 3, -1,
                        -1,-1, 2, -1, 0, 3,
                        -1, 5 ,-1, 2, 4, 0};
    // allocate space for A
    printf("Writing hardcoded adjacency matrix A[%d][%d] to '%s'...\n", n, n, outfile);   
    ret = write_graph(A, n, outfile);   // write A to output file
end:
    exit(ret);
}
