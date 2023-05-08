/**
 * @file floyds.c
 * @author Leslie Horace
 * @brief Main program for performing the floyds shortest path algorithm
 * @version 1.0
 * 
 */
#include "utilities.h"   

void floyds_serial(GraphShared gs){
    // loop each node and find min path
    for(int k = 0; k < gs.n; k++){ 
        for(int i = 0; i < gs.n; i++){ 
            find_paths(gs.A, gs.n, i, k);
        }
    }
}

int main(int argn, char **argv) {
    int ret = 1;
    double start_overall = 0.0, end_overall = 0.0, start_compute = 0.0, end_compute = 0.0;;
    GET_TIME(start_overall);
    // check if 5 args were entered
    if (argn != 3){
        printf("Usage: %s <in_file> <out_file>\n", argv[0]);
        goto end_all;
    }
    GraphShared gs = {NULL, 0};
    char *infile = argv[1], *outfile = argv[2];
    // read in the adjacency matrix, end if error
    printf("Reading adjacency matrix from '%s'\n", infile);
    ret = read_graph(&gs.A, &gs.n, infile);
    if(ret != 0) goto end_all; 
    // perform floyds algorithm
    printf("Running floyds algorithm in serial...\n");
    GET_TIME(start_compute);
    floyds_serial(gs);
    GET_TIME(end_compute);
    // write results to outfile
    printf("Writing result matrix A[%dx%d] to '%s'\n", gs.n, gs.n, outfile);
    if((ret = write_graph(gs.A, gs.n, outfile)) != 0) goto end_a;
    GET_TIME(end_overall);
    print_times(end_overall - start_overall, end_compute-start_compute);
end_a:
    free(gs.A);
end_all:
    exit(ret); 
}