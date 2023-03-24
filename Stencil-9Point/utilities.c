/**
 * @file utilities.c
 * @author Leslie Horace
 * @brief File storing all user functions used in the project
 * @version 1.0
 * 
 */
#include "utilities.h"

int malloc1D(double **X, int n, int m, char * var_name) {
    double *Y = (double *)malloc(n*m*sizeof(double));   // malloc new 1-dim matrix
    if (Y == NULL){ // check if null and print error
        printf("Error [utilities] => malloc1D():  cannot allocate space for %s[%zu]\n", var_name, n*m*sizeof(double));
        return 1;
    }
    *X=Y;  // check if null and print error
    return 0;
}

int mallocPtr(void**P, size_t size, char * ptr_name) {
    void *Q = (void*)malloc(size);   // malloc an array of pointers
    if (Q == NULL){ // check if null and print error
        printf("Error [utilities] => mallocPtr(): cannot allocate space for %s[%zu]\n", ptr_name, size);
        return 1;
    }
    *P=Q;  // check if null and print error
    return 0;
}

void init2D(double *X, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 1; j < m-1; j++){
            X[i*m+j] = 0;        // set each middle col to 0
        }
        X[i*m] = X[(i*m)+(m-1)] = 1;        // set first and last col to 1
    }
}   

void print2D(double *X, int n, int m){
    printf("\n");
    for(int i =0; i < n; i ++){
        for(int j = 0; j < m; j ++){
            printf("%.2f\t", X[i*m+j]);     // prints each element
        }printf("\n");
    }
}

void swap2D(double **X, double **Y){
        double *Z = *Y;        // temp pointer = target matrix
        *Y=*X;      // target = source
        *X=Z;       // source = target
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

int handleIOError(FILE * fptr, size_t count, size_t expected){
    // no error reading/writing
    if(expected == count) return 0;
    // error occured reading/writing
    if(ferror(fptr)){   // handle specific file errors
        perror("Error [utilities] => handleIOError()");
    }else if(feof(fptr)){   // handle end of file error
        perror("Error [utilities] => handleIOError()");
    }
    return 1;   
}

int read2D(double **X, int *n, int *m, char * infile){
    FILE * fp = NULL; 
    int ret = 1;
    // check if file is open for reading
    if ((fp = fopen(infile, "rb")) == NULL){
        perror("Error [utilities] => read2D() => fopen()");
        return ret;
    }
    // read in matrix order metadata
    if(fread(&(*n), sizeof(int), 1, fp) == 1 && fread(&(*m), sizeof(int), 1, fp) == 1){
        // allocate space for n*m matrix
        if((ret == malloc1D(X, (*n), (*m), "X")) == 0){
            // read infile contents into matrix 
            size_t count = fread(X[0], sizeof(double), (*n)*(*m), fp);     
            // check if all data read from file  
            ret = handleIOError(fp, count, (size_t)(*n)*(size_t)(*m));
        }
    }   
    fclose(fp); 
    return ret;
}

int write2D(double *X, int n,  int m, char * outfile){
    FILE * fp = NULL; 
    int ret = 1;
    // check if file is open for writing
    if ((fp = fopen(outfile, "wb")) == NULL){
        perror("Error [utilities] => write2D() => fopen()");
        return ret;
    }
    // write matrix order metadata
    if(fwrite(&n, sizeof(int), 1, fp) == 1 &&  fwrite(&m, sizeof(int), 1, fp) == 1){
        size_t count = fwrite(X, sizeof(double), n*m, fp);
        // check if all data wrote to file
        ret = handleIOError(fp, count, (size_t)n*(size_t)m);
    }
    fclose(fp);
    return ret;
}


void stencil2D(double *X, double *Y, int n , int m){
    for(int i = 1; i < n-1; i++){
        for(int j = 1; j < m-1; j++){
            // X[C] = (Y[NW]+Y[N]+Y[NE]+Y[E]+Y[SE]+Y[S]+Y[SW]+Y[W]+Y[C])/9.0
            X[i*m+j] = (Y[(i-1)*m+j-1] + Y[(i-1)*m+j] + Y[(i-1)*m+j+1] 
            + Y[i*m+j+1] + Y[(i+1)*m+j+1] + Y[(i+1)*m+j] 
            + Y[(i+1)*m+j-1] + Y[i*m+j-1] + Y[i*m+j])/9.0;
        }
    }
}

void blockStencil2D(double *X, double *Y, int block_start, int block_size, int m){
    // same as stencil2D, but only iterates blocks of rows
    for(int i = block_start; i < (block_start+block_size); i++){
        for(int j = 1; j < m-1; j++){
            X[i*m+j] = (Y[(i-1)*m+j-1] + Y[(i-1)*m+j] + Y[(i-1)*m+j+1] 
            + Y[i*m+j+1] + Y[(i+1)*m+j+1] + Y[(i+1)*m+j] 
            + Y[(i+1)*m+j-1] + Y[i*m+j-1] + Y[i*m+j])/9.0;
        }
    }   
}

