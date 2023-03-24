/**
 * @file utilities.h
 * @author Leslie Horace  (lahorace@coastal.edu)
 * @brief Header file for structs and functions in functions.c
 * @version 2.0 
 * 
 */
#define _GNU_SOURCE
#include <stdlib.h>     
#include <stdio.h>  
#include <errno.h>
#include <pthread.h>
#include "timer.h"
#ifndef UTILITIES_
#define UTILITIES_

#define MIN(a,b)  ((a)<(b)?(a):(b))     // min value between a and b
#define UNIF(l,u) (l+((int)((u-l)*drand48())))      // uniform random number, st. lb <= x < ub
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))       // thread block starting index 
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)    // thread block ending index
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)     // threads block size 
#define RAND_SEED 1     // constant for seeding drand48()

/**
 *  @brief  Shared struct for graph data
 *  @typedef GraphShared
 *  @param A (int*) matrix
 *  @param n (int) # rows/cols
*/
typedef struct _GraphShared{
    int *A;
    int n;
}GraphShared;

/**
 *  @brief  Shared struct for thread data
 *  @typedef ThreadShared
 *  @param  compute_time (double) total compute time
 *  @param barrier (pthread_barrier_t) thread barrier for synchonization 
*/
typedef struct _threadShared{
    double compute_time;
    pthread_barrier_t barrier;
}ThreadShared;

/**
 *  @brief  Private data for each thread
 *  @typedef ThreadPrivate
 *  @param gs_ptr (GraphShared *) ptr to GraphShared struct
 *  @param ts_ptr (ThreadShared *) ptr to ThreadShared struct
 *  @param block_start (int) thread start row
 *  @param block_size (int) # rows in thread block 
 *  @param thread_time (double) time for each thread
*/
typedef struct _ThreadPrivate{
    GraphShared * gs_ptr;
    ThreadShared * ts_ptr;
    int block_start;
    int block_size;
    double thread_time;
}ThreadPrivate;

/** 
 *  @brief  Allocates space of vany type size and returns a pointer
 *  @param P (void**) pointer for mallocing
 *  @param size (size_t) size to malloc [num_elements*sizeof(type)]
 *  @param ptr_name (char*) name of pointer 
 *  @return [args]: P; [value]: 1 = Error ||  0 = Success
 */
int malloc_anything(void**P, size_t size, char * ptr_name);

/**
 *  @brief Generates a random adjacency matrix
 *  @param gs (GraphShared) struct for graph data
 *  @param p (double) probability a node is connected
 *  @param lb (int) lower bound
 *  @param ub (int) upper bound
*/
void gen_graph(GraphShared gs, double p, int lb, int ub);

/**
 * @brief Prints a matrix to the console
 * @param gs (GraphShared) struct for graph data
*/
void print_graph(GraphShared gs);

/**
 * @brief Prints times for overall, other/io, and compute
 * @param overall_time (double) time spent overall in the program 
 * @param compute_time (double) time spent doing computations
*/
void print_times(double overall_time, double compute_time);

/**
 * @brief  finds shortest path in each row i from node k
 * @param X (int**) Adjacency matrix 
 * @param n (int) # cols
 * @param k (int) current node
 */
void find_paths(int *X, int n, int i, int k);

/**
 * @brief Reads (.dat) file as binary data into memory
 * @param X (int**) Matrix for reading
 * @param n (int) # rows, # cols
 * @param infile Input filename (.dat)
 * @return [args]: X, n; [value]: 1 = Error ||  0 = Success
 */
int read_graph(int **X, int *n, char * infile);

/**
 * @brief Writes matrix from memory as binary data into file
 * @param X (int**) Matrix for writing
 * @param n (int) # rows, # cols
 * @param outfile Input filename (.dat)
 * @return [value]: 1 = Error (malloc) || 0 = Success 
 */
int write_graph(int *X, int n, char * outfile);

/**
 * @brief Validates integer arguments from cmdline
 * @param arg_ptr (char*) integer argument to parse
 * @param arg_min (int) min valid integer
 * @param arg_name (char*) argument usage name
 * @return [value]:  parsed_num || -1 = Error
 */
int parse_int(char * arg_ptr, int arg_min, char * arg_name);

/**
 *  @brief Check for specific file errors for fwrite and fread
 *  @param fptr file stream pointer to check
 *  @param count (size_t) count returned from fread() or fwrite()
 *  @param expected (size_t) count of elements read orwritten
 *  @return [value]: 1 = Error || 0 = success
*/
int handle_IO(FILE * fptr, size_t count, size_t expected);

/**
 *  @brief Check if valid thread reached the call to pth_barrier_wait()
 *  @param retval return value from pth_barrier_wait()
 *  @return 1 = Error | 0 = success
*/
int handleBarrier(int retval);

#endif /* UTILITIES_ */