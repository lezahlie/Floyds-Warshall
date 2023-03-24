/**
 * @file utilities.h
 * @author Leslie Horace
 * @brief Header file for structs and functions in functions.c
 * @version 1.0 
 * 
 */
#define _GNU_SOURCE
#include <stdlib.h>     
#include <stdio.h>  
#include <pthread.h>
#include <errno.h>
#include "timer.h"
#ifndef UTILITIES_
#define UTILITIES_

#define MIN(a,b)  ((a)<(b)?(a):(b))     // returns the min value between a and b
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))   // return threads block starting index 
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)    // return threads block ending index
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)     // returns threads block size 
#define BtoGB(bytes) (long double)(bytes*(0.1*10e-9))   // converts bytes to GB for debugging
#define FALSE 0
#define TRUE 1


/** 
 *  @struct _matrixData
 *  @typedef MatrixData (shared)
 *  @brief  struct for all shared variables for matrices
 */
typedef struct _matrixData{
    double * A;
    double *B;
    int rows;
    int cols;
}MatrixData;

/** 
 *  @struct _fileData
 *  @typedef FileData (shared)
 *  @brief  struct for all shared file name handlers
 */
typedef struct _fileData{
    char * initfile;
    char * finalfile;
    char * allfile;
    int writeRaw;
}FileData;

/** 
 *  @struct _stencilData
 *  @typedef StencilData (shared)
 *  @brief  struct for all shared variables needed in stencil interation loop
 */
typedef struct _stencilData{
   int num_iter;
   int num_threads;
   int debug_level;
   double total_cpu;
   pthread_barrier_t barrier;
}StencilData;

/** 
 *  @struct _threadData
 *  @typedef ThreadData (private)
 *  @brief thread struct holding shared struct pointers and private variables
 */
typedef struct _threadData{
    StencilData * s_data;         
    MatrixData * m_data;     
    FileData * f_data;           
    int block_id;               
    int block_start;
    int block_size;
    double thread_cpu; 
}ThreadData;

/** 
 *  @brief  Allocates space for an n*m double pointer matrix
 *  @param P (double**) Matrix to initalize
 *  @param size size to malloc [n*sizeof(type)]
 *  @return &P , 1 = Error | 0 = Success 
 */
int mallocPtr(void ** P, size_t size, char * ptr_name);

/**
 *  @brief  Allocates space for an n*m 1-dim matrix
 *  @param X (double**) Matrix to initalize
 *  @param n # rows
 *  @param m # columns
 *  @return &X, , 1 = Error | 0 = Success 
*/
int malloc1D(double **X, int n, int m, char * var_name);

/**
 *  @brief Initilaizes a matrix in memory for 9-pt stencil operations
 *  @param X (double**) Matrix to initalize
 *  @param n # rows
 *  @param m # columns
*/
void init2D(double *X, int n, int m);

/**
 *  @brief Prints a matrix to the console
 *  @param X (double**) Matrix to be printed 
 *  @param n # rows
 *  @param m # columns
*/
void print2D(double *X, int n, int m);

/**
 *  @brief Swaps addresses of two matrix pointers
 *  @param X Source (double**) matrix 
 *  @param Y Target (double**) matrix 
 *  @return &X, &Y
*/
void swap2D(double **X, double ** Y);


/**
 *  @brief Check if valid thread reached the call to pth_barrier_wait()
 *  @param retval return value from pth_barrier_wait()
 *  @return 1 = Error | 0 = success
*/
int handleBarrier(int retval);

/**
 *  @brief Check for specific file errors for fwrite and fread
 *  @param fptr file stream pointer to check
 *  @param count (size_t) count returned from fread() or fwrite()
 *  @param expected (size_t) count of elements read orwritten
 *  @return 1 = Error | 0 = success
*/
int handleIOError(FILE * fptr, size_t count, size_t expected);

/**
 * @brief Reads a (.dat) file as binary data into matrix in memory
 * @param X (double**) Matrix for reading
 * @param n (int) # rows
 * @param m (int) # columns
 * @param infile Input filename (.dat)
 * @return &X, &n, &m, 1 = Error | 0 = Success 
 */
int read2D(double **X, int *n, int *m, char * infile);

/**
 * @brief Writes matrix from memory as binary data into a (.dat) file
 * @param X (double**) Matrix for writing
 * @param n (int) # rows
 * @param m (int) # columns
 * @param outfile Input filename (.dat)
 * @return 1 = Error | 0 = Success 
 */
int write2D(double *A, int n,  int m, char * outfile);

/**
 * @brief Algorithm to perform a 9-pt stencil operation
 * @param X (double**) Matrix being modified
 * @param Y (double**) Matrix for computations
 * @param n (int) # rows
 * @param m (int) # columns
 */
void stencil2D(double *X, double *Y, int n , int m);

/**
 * @brief Algorithm to perform a 9-pt stencil operation by blocks
 * @param X (double**) Matrix being modified
 * @param Y (double**) Matrix for computations
 * @param block_start (int)  starting row
 *  @param block_size (int) # rows in block
 * @param m (int) # columns
 */
void blockStencil2D(double *X, double *Y, int block_start, int block_size, int m);

#endif /* UTILITIES_ */