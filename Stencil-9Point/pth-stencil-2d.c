/**
 * @file pth-stencil-2d.c
 * @author Leslie Horace
 * @brief Main program for performing and recording 9-pt serial stencil operations
 * @version 1.0
 */
#include "utilities.h"

void * pthStencilLoops(void* td_ptr){
    ThreadData * td = td_ptr;
    FILE * fp =NULL;
    double start_cpu = 0.0, end_cpu = 0.0;
    size_t count = 0;
    int ret = 0;
    // the last thread will handle file writing and debug printing
    if(td->block_id == 0){
        // check if debugging is level 2 for printing matrix state
        if(td->s_data->debug_level == 2) print2D(td->m_data->B, td->m_data->rows, td->m_data->cols);
        // check if writeRaw option is true for stacked raw file
        if(td->f_data->writeRaw == TRUE){
            // open the file for writing and check for errors
            if ((fp = fopen(td->f_data->allfile, "wb")) == NULL){
                printf("Error: cannot open/write to '%s'\n", td->f_data->allfile);
                td->f_data->writeRaw = FALSE;   // disable writing to raw file on error
            }else{
                // write to stacked raw file and check for errors
                count = fwrite(td->m_data->B, sizeof(double), td->m_data->rows*td->m_data->cols, fp);
                if(handleIOError(fp, count, (size_t)td->m_data->rows*(size_t)td->m_data->cols) == 1){
                    td->f_data->writeRaw = FALSE;       // disable writing to raw file on error
                    fclose(fp);     // close the file pointer
                }
            }
        }
    }
    // start blocked stencil iterations 
    for(int i = 0; i< td->s_data->num_iter; i++){
        GET_TIME(start_cpu);    // start clock
        // perform blocked stencil algorithm
        blockStencil2D(td->m_data->A, td->m_data->B, td->block_start, td->block_size, td->m_data->cols);      
        GET_TIME(end_cpu);  // end clock
        // sum thread cpu time for each iteration
        td->thread_cpu += (end_cpu-start_cpu);   
        // wait for all threads to finish this iteration andbreak if error
        ret = pthread_barrier_wait(&td->s_data->barrier);
        if(handleBarrier(ret) == 1) break; 
        // check if last thread has finished its stencil block 
        if(td->block_id == 0){
            // print matrix state if debug level is 2
            if(td->s_data->debug_level ==2) print2D(td->m_data->A, td->m_data->rows, td->m_data->cols);
            // check if writeRaw option is true for stacked raw file
            if(td->f_data->writeRaw == TRUE){
                // write to stacked raw file and check for errors
                count = fwrite(td->m_data->A, sizeof(double), td->m_data->rows*td->m_data->cols, fp);
                if(handleIOError(fp, count, (size_t)td->m_data->rows*(size_t)td->m_data->cols) == 1){
                    td->f_data->writeRaw = FALSE;       // disable writing to raw file on error
                    fclose(fp);     // close the file pointer
                }
            }
            // swap matrix pointers for next iteration
            swap2D(&td->m_data->A, &td->m_data->B);   
        }
        // wait for all threads to finish this iteration and break on error
        ret = pthread_barrier_wait(&td->s_data->barrier);
        if(handleBarrier(ret) == 1) break; 
    }
    if(MIN(td->s_data->total_cpu, td->thread_cpu) != td->thread_cpu) td->s_data->total_cpu = td->thread_cpu;
    // let last thread close file handler if writeRaw option is true
    if(td->block_id == 0 && td->f_data->writeRaw == TRUE) fclose(fp);
    return NULL;
}


int main(int argc, char **argv) {
    int ret = 1; 
    double start_overall = 0.0, end_overall = 0.0, time_overall = 0.0;
    GET_TIME(start_overall);
    // check if 6 or 7 args were entered
    if (argc < 6 || argc > 7){ 
        printf("Usage: %s <num iter.> <infile> <outfile> <debug level [0-2]> <num threads> <all-stacked-file-name.raw (optional)>\n", argv[0]);
        exit(ret);
    }
    // initalize file data 
    FileData fd = {argv[2],argv[3], argv[6], (argc == 7) ? (TRUE) : (FALSE)};
    // initialize stencil data struct 
    StencilData sd = {.num_iter=0,.num_threads=0,.debug_level=0,.total_cpu=0.0};
    // parse <num iter.> and < num_threads> arguments as base 10 int
    char * iptr,  *dptr, * tptr; 
    sd.num_iter = strtol((char *)argv[1], &iptr, 10);
    sd.debug_level = strtol((char *)argv[4], &dptr, 10);
    sd.num_threads = strtol((char *)argv[5], &tptr, 10);
    // check if <num iter.> < num_threads> <debug level [0-2] are valid 
    if(*iptr != '\0' || *tptr != '\0' ||   sd.num_threads < 1 || sd.num_iter < 1){
        printf("Error [pth-stencil-2d] => main(): <num iter.> and <num threads> must be a postive non zero integers\n"); 
        exit(ret);
    }else if(*dptr != '\0' || sd.debug_level < 0 || sd.debug_level > 2){
        printf("Error [pth-stencil-2d] => main():  <debug level [0-2]> must be in range of [0-2]\n"); 
        exit(ret);
    }
   // initialize matrix struct
    MatrixData md = {NULL, NULL, 0, 0};
    // read in matrix A from infile and check for errors
    if((ret = read2D(&md.A, &md.rows, &md.cols, fd.initfile))==1) exit(ret);
    // check if threads entered can be evenly divided by blockable rows
    if(MIN(sd.num_threads,md.rows-2) != sd.num_threads){
        printf("Error [pth-stencil-2d] => main(): num_threads[%d] > blockable rows[%d] in stencil operations\n", sd.num_threads, md.rows-2);
        free(md.A); 
        exit(ret);
    }
    // malloc space for duplicate matrix B and check for errors
    if((ret = malloc1D(&md.B, md.rows, md.cols, "md.B"))==1){
        free(md.A);
        exit(ret);
    }
    // initialize thread handles and thread data struct pointers
    pthread_t * th_handles = NULL;
    ThreadData * td = NULL, * td_tmp = NULL;
    // malloc thread_handles and check for errors
    if((ret= mallocPtr((void*)&th_handles, sd.num_threads*sizeof(pthread_t), "th_handles"))==1){
        free(md.A); free(md.B);
        exit(ret);
    }
    // malloc thread data struct and check for errors
    if((ret= mallocPtr((void*)&td, sd.num_threads*sizeof(ThreadData), "td"))==1){
        free(md.A); free(md.B); free(th_handles);
        exit(ret);
    }
    // initialize pthread barrier with number of threads and check for error
    if((ret=pthread_barrier_init(&sd.barrier,NULL, sd.num_threads)) != 0){
        free(md.A); free(md.B); free(th_handles);
        errno = ret;
        perror("Error [pth-stencil-2d] => main() => pthread_barrier_init()");
        exit(ret);
    }
    init2D(md.B, md.rows, md.cols);    // initialize matrix B
    td_tmp = td;    // save starting address to temp pointer
    if(sd.debug_level > 0) printf("Running %d parallel stencil iterations...\n", sd.num_iter);
    // loop to save each threads private data and shared data pointer, then create threads
    for(int tid = 0; tid < sd.num_threads; tid++){
        td->s_data= &sd;
        td->m_data=&md;
        td->f_data=&fd;
        td->block_id = tid;
        td->block_start =BLOCK_LOW(tid, sd.num_threads, md.rows-2)+1;
        td->block_size = BLOCK_SIZE(tid, sd.num_threads, md.rows-2);
        td->thread_cpu = 0.0;
        // create each thread, pass thread data struct, and void function, check for errors
        if((ret = pthread_create(&th_handles[tid], NULL, pthStencilLoops, (void*)td)) != 0){
            errno = ret;
            perror("Error [pth-stencil-2d] => main() => pthread_create()");
            sd.num_threads = tid+1; // set number of threads for joining only created threads
            break;  // break thread creation since we are missing threads
        } 
        td++;
    }
    // join each thread to the parent thread and check for errors
    for (int tid = 0;  tid < sd.num_threads; tid++){
        if((ret = pthread_join(th_handles[tid], NULL)) !=0){
            errno = ret;
            perror("Error [pth-stencil-2d] => main() => pthread_join()");
        }
    }
    // destroy the barrier and check for any errors
    if((ret = pthread_barrier_destroy(&sd.barrier)) != 0){
        perror("Error [pth-stencil-2d] => main() => pthread_barrier_destroy()");
    }
    // write the final files data
    ret = write2D(md.B, md.rows, md.cols, fd.finalfile);
    if(sd.debug_level > 0){
        if(ret == 0){
            printf("------------------------------------------------------------------\n");
            long finalfile_size = ((long)md.rows*(long)md.cols*sizeof(double))+((long)2*(sizeof(int)));
            printf("Wrote final matrix state to '%s'\n[%s] size = %ld(B) = %.6Lg(GB)\n", fd.finalfile,fd.finalfile, finalfile_size, BtoGB(finalfile_size));
        }
        if(fd.writeRaw == TRUE){
            printf("------------------------------------------------------------------\n");
            long allfile_size = (long)md.rows*(long)md.cols*sizeof(double)*((long)sd.num_iter+1);
            printf("Wrote inital matrix state + %d iterations to '%s'\n[%s] size = %ld(B) = %.6Lg(GB)\n", sd.num_iter, fd.allfile, fd.allfile, allfile_size, BtoGB(allfile_size));
        }
    }
    // restore start addr to thread data struct and free all mallocs
    td = td_tmp;        
    // free all mallocs
    free(th_handles);
    free(td);
    free(md.B);
    free(md.A);
    // calculate times and print
    GET_TIME(end_overall);
    time_overall = end_overall-start_overall;
    printf("------------------------------------------------------------------\n");
    printf("[Elapsed Time] = %g sec\n[Other Time] = %g sec\n[Compute Time] = %g sec", time_overall, (time_overall - sd.total_cpu), sd.total_cpu);
    printf("\n------------------------------------------------------------------\n");
    exit(ret); 
}
