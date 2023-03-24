/**
 * @file pth-floyds.c
 * @author Leslie Horace
 * @brief Main program for performing the floyds shortest path algorithm
 * @version 1.0
 * 
 */
#include "utilities.h"   

void * pth_floyds(void * tp_ptr){
    ThreadPrivate * tp = tp_ptr;
    double start_compute= 0.0, end_compute = 0.0;
    int ret = 0;
    // run floyds alg for current node k
    for(int k = 0; k < tp->gs_ptr->n; k++){ 
        GET_TIME(start_compute);
        for(int i = tp->block_start; i < (tp->block_start+tp->block_size); i++){
            find_paths(tp->gs_ptr->A, tp->gs_ptr->n, i, k);
        }
        GET_TIME(end_compute);
        // get the threads total time doing computations
        tp->thread_time += (end_compute-start_compute);
        // wait for all threads to finish before next iteration
        ret = pthread_barrier_wait(&tp->ts_ptr->barrier);
        if(handleBarrier(ret) == 1) break; 
    }
    // set compute time to the longest thread time
    if(MIN(tp->ts_ptr->compute_time, tp->thread_time) != tp->thread_time){
        tp->ts_ptr->compute_time = tp->thread_time;
    }
    return NULL;
}

int main(int argn, char **argv) {
    int ret = 1, num_threads = 0;
    double start_overall = 0.0, end_overall = 0.0;

    GET_TIME(start_overall);
    // check if 5 args were entered
    if (argn != 4){
        printf("Usage: %s <in_file> <out_file> <num_threads>\n", argv[0]);
        goto end_all;
    }

    // parse num_threads argument
    if((num_threads = parse_int(argv[3], 1, "num_threads")) == -1) goto end_all;
    // read in the adjacency matrix, end if error
    GraphShared gs = {NULL, 0};
    char *infile = argv[1], *outfile = argv[2];
    printf("Reading adjacency matrix from '%s'\n", infile);
    if((ret = read_graph(&gs.A, &gs.n, infile)) != 0) goto end_all; 
    if(num_threads > gs.n){
        printf("Error [%s]: num_threads [%d] is greater than blockable rows [%d]\n", argv[0], num_threads, gs.n);
        goto end_a;
    }

    // malloc thread_handles, decallocate and end on error
    pthread_t * thread_handles = NULL;
    if((ret = malloc_anything((void*)&thread_handles, num_threads*sizeof(pthread_t), "thread_handles")) == 1){
        goto end_a;
    }

    // malloc thread private struct, decallocate and end on error
    ThreadPrivate * tp = NULL, * tp_tmp = NULL;
    if((ret = malloc_anything((void*)&tp, num_threads*sizeof(ThreadPrivate), "tp")) == 1){
        goto end_b;
    }
    tp_tmp = tp;    // save start addr 

    // initialize pthread barrier with number of threads and check for error
    ThreadShared ts = {.compute_time=0.0};
    if((ret=pthread_barrier_init(&ts.barrier,NULL, num_threads)) != 0){
        errno = ret;
        perror("Error [pth_floyds] -> pthread_barrier_init()");
        goto end_c;
    }

    // perform floyds algorithm
    printf("Running floyds algorithm with %d thread(s)...\n", num_threads);
    for(int rank = 0; rank < num_threads; rank++){
        tp->gs_ptr = &gs;
        tp->ts_ptr= &ts;
        tp->block_start =BLOCK_LOW(rank, num_threads, gs.n);
        tp->block_size = BLOCK_SIZE(rank, num_threads, gs.n);
        tp->thread_time = 0.0;
         if((ret = pthread_create(&thread_handles[rank], NULL, pth_floyds, (void*)tp)) != 0){
            errno = ret;
            perror("Error [pth-floyds] -> pthread_create()");
            num_threads = rank+1; // set number of threads for joining only created threads
            break;  // break thread creation since we are missing threads
        } 
        tp++;
    }

    // join each thread to the parent thread and check for errors
    for (int rank = 0;  rank < num_threads; rank++){
        if((ret = pthread_join(thread_handles[rank], NULL)) !=0){
            errno = ret;
            perror("Error [pth_floyds] -> pthread_join()");
        }
    }

    // destroy the barrier and check for any errors
    if((ret = pthread_barrier_destroy(&ts.barrier)) != 0){
        errno = ret;
        perror("Error [pth-stencil-2d] => main() => pthread_barrier_destroy()");
        goto end_c;
    }

    // write results to outfile
    printf("Writing result matrix A[%dx%d] to '%s'\n", gs.n, gs.n, outfile);
    if((ret = write_graph(gs.A, gs.n, outfile)) == 1) goto end_c;
    // print overall, other/io, and compute times
    GET_TIME(end_overall);
    print_times(end_overall-start_overall, ts.compute_time);

end_c: 
    tp = tp_tmp;    //restore start addr
    free(tp);
end_b:    
    free(thread_handles);
end_a:    
    free(gs.A);
end_all:
    exit(ret); 
}