/**
 * @file utilities.c
 * @author Leslie Horace
 * @brief File storing all user functions used in the project
 * @version 2.0
 * 
 */
#include "utilities.h"

int malloc_anything(void**P, size_t size, char * ptr_name) {
    void *Q = (void*)malloc(size);   // malloc an array of pointers
    if (Q == NULL){ // check if null and print error
        printf("Error [utilities] => mallocPtr(): cannot allocate space for %s[%zu]\n", ptr_name, size);
        return 1;
    } *P=Q;   // here on success, set starting address 
    return 0;
}

void gen_graph(GraphShared gs, double p, int lb, int ub){
    for(int i = 0; i < gs.n; i++){
        gs.A[i*gs.n+i] = 0; // set diagonal to 0
        for(int j = 0; j < gs.n; j++){
            if(i==j) continue;  // skip diagnols
            gs.A[i*gs.n+j] = (drand48() > p)    // if the edge is not connected
            ? -1 : UNIF(lb,ub);    // set edge to -1, otherwise generate random weight
        }
    }
}   

void print_graph(GraphShared gs){
    for(int i =0; i < gs.n; i ++){
        for(int j = 0; j < gs.n; j ++){
            // print each elenent
            printf("%d\t", gs.A[i*gs.n+j]);   
        }printf("\n");
    }
}

void print_times(double overall_time, double compute_time){
    printf("-------------------------------------------------\n");
    printf("[Overall Time] = %g sec\n[I/O Time] = %g sec\n[Compute Time] = %g sec", 
    overall_time, (overall_time - compute_time), compute_time);
    printf("\n-------------------------------------------------\n");
}

void find_paths(int *X, int n, int i, int k){
        for(int j = 0; j < n; j++){
            // skip 0 edges (self/joined nodes) and -1 edges (not connected)
            if(X[i*n+j] == 0 || X[i*n+k] == -1 || X[k*n+j] == -1) continue;   
            X[i*n+j] = ((X[i*n+j] > 0)     // if edge X[i][j] exists
                ? MIN(X[i*n+j], X[i*n+k] + X[k*n+j])    // then X[i][j] = MIN(X[i][j], X[i][k] + X[k][j])
                : (X[i*n+k] + X[k*n+j]));   // otherwise X[i][j] = X[i][k] + X[k][j]
        }
}

void blocked_find_paths(int *X, int start_row, int num_rows, int n, int k){
    for(int i = start_row; i < start_row+num_rows; i++){
        find_paths(X, n, i, k);
    }
}

int read_graph(int **X, int *n, char * infile){
    FILE * fp = NULL; 
    int ret = 1;
    // check if file is open for reading
    if ((fp = fopen(infile, "rb")) == NULL){
        perror("Error [utilities] -> read_graph()");
        return ret;
    }
    // read in matrix order metadata
    if(fread(&(*n), sizeof(int), 1, fp) == 1){
         // allocate space for n*n natrix
        if((ret == malloc_anything((void*)X, ((size_t)(*n)*(size_t)(*n))*sizeof(int), "X")) == 0){
            // read infile contents into matrix 
            size_t count = fread(X[0], sizeof(int), (*n)*(*n), fp);     
            // check if all data read from file  
            ret = handle_IO(fp, count, (size_t)(*n)*(size_t)(*n));
        }
    }
    fclose(fp); 
    return ret;
}

int write_graph(int *X, int n, char * outfile){
    FILE * fp = NULL; 
    int ret = 1;
    // check if file is open for writing
    if ((fp = fopen(outfile, "wb")) == NULL){
        perror("Error [utilities] -> write_graph()");
        return ret;
    }
    // write natrix order metadata
    if(fwrite(&n, sizeof(int), 1, fp) == 1){
        size_t count = fwrite(X, sizeof(int), n*n, fp);
        // check if all data wrote to file
        ret = handle_IO(fp, count, (size_t)n*(size_t)n);
    }
    fclose(fp);
    return ret;
}

int parse_int(char *arg_ptr, int arg_min, char *arg_name){
    char * tmp_ptr = NULL;  
    // parse the arg as base 10 int
    int tmp_num = strtol((char *)arg_ptr, &tmp_ptr, 10);  
    // check if arg is an integer and at least the specified min 
    if (*tmp_ptr != '\0' ||  tmp_num < arg_min){
        printf("Error [utilities] -> parse_int(): <%s> %s must be an integer at least %d\n", arg_name, arg_ptr, arg_min);
        return -1;
    }
    return tmp_num;
}

int handle_IO(FILE * fptr, size_t count, size_t expected){
    // no error reading/writing
    if(expected == count) return 0;
    // error occured reading/writing
    if(ferror(fptr)){   // handle specific file errors
        perror("Error [utilities] -> handle_IO()");
    }else if(feof(fptr)){   // handle end of file error
        perror("Error [utilities] -> handle_IO()");
    }
    return 1;   
}

int handleBarrier(int retval){
        // on success 1 thread returns the below macro and the rest return 0
        if(retval != PTHREAD_BARRIER_SERIAL_THREAD && retval != 0){
        errno = retval;
        perror("Error [utilities] => handleBarrier()");
        return 1;
    }
    return 0;
}
