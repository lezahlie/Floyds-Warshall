/**
 * @file make-graph.c
 * @author Leslie Horace
 * @brief Main program to create random adjacency matrix and write to a file
 * @version 1.0
 * 
 */
#include "utilities.h"

int main(int argn, char **argv) {
    int ret = 1;
    // check if all args were entered 
    if (argn != 6){ 
        printf("Usage: %s <N> <file_name> <P> <L> <U>\n", argv[0]);
        goto end;
    }
    GraphShared gs = {NULL, 0};
    char *tmp = NULL, *outfile = argv[2];
    int lb = 0, ub = 0; 
    // parse arguments <N> <P> <L> <U> and validate
    if((gs.n = parse_int(argv[1], 1, "N")) == -1) goto end;
    double p = strtod((char *)argv[3], &tmp);
    if(*tmp != '\0' || p < 0.0 ||  p > 1.0){
        printf("Error [%s]: <P> %s must be a double in range [0.0,1.0]\n", argv[0], argv[3]);
        goto end;
    }
    if((lb = parse_int(argv[4], 1, "L")) == -1) goto end;
    if((ub = parse_int(argv[5], lb, "U")) == -1) goto end;
    // allocate space for A, end if error
    if((ret=malloc_anything((void*)&gs.A, ((size_t)gs.n*(size_t)gs.n)*sizeof(int), "A") !=0)) goto end;
    // seed drand48() for init_graph()   
    srand48(RAND_SEED);    
    // generate random adjacency matrix A
    gen_graph(gs, p, lb, ub+1);   
    // write A to output file
    printf("Writing random adjacency matrix A[%d][%d] to '%s'...\n", gs.n, gs.n, outfile);   
    ret = write_graph(gs.A, gs.n, outfile); 
    free(gs.A);
end:
    exit(ret);
}
