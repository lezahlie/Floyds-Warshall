/**
 * @file stencil-2d.c
 * @author Leslie Horace
 * @brief Main program for performing and recording 9-pt serial stencil operations
 * @version 1.0
 * 
 */
#include "utilities.h"   

int stencilAll(MatrixData md, FileData fd, int n_iter, double * io_time){
    int ret = 0;
    FILE * fp;
    size_t count = 0;
    double start_io=0.0, end_io=0.0;
    // check if file is open for writing
    if((fp = fopen(fd.allfile, "wb")) == NULL){ 
        printf("File error: cannot open/write to '%s'\n", fd.allfile);
        fd.writeRaw = FALSE;
        ret =1;
    }else{
        // write initial matrix to stacked raw file and check for errors
        count = fwrite(md.A, sizeof(double), md.rows*md.cols, fp);
        if((ret = handleIOError(fp, count, (size_t)(md.rows*md.cols))) == 1){
            fd.writeRaw = FALSE;
            fclose(fp);
        }
    }
    // write the initial matrix to output file 
    for(int i=0; i < n_iter; i++){         
        stencil2D(md.A, md.B, md.rows, md.cols);                // perform stencil iterations 
        // record/sum io time and write stencil iteration result to raw file
        GET_TIME(start_io); 
        // check if stacked file is still open for writing
        if(fd.writeRaw == TRUE){
            // if true, write each iteration to raw file and check for error
            count = fwrite(md.A, sizeof(double), md.rows*md.cols, fp);
            if((ret = handleIOError(fp, count, (size_t)(md.rows*md.cols))) == 1){
                fd.writeRaw = FALSE;  // set writeRaw flag to false if error
                fclose(fp);     // close the file pointer
            }
        }
        GET_TIME(end_io);
        *io_time += (end_io-start_io);  
        // swap matrices for next iteration
        swap2D(&md.A, &md.B);  
    }
    // close the file pointer for stacked raw file
    if(fd.writeRaw == TRUE) fclose(fp);
    // record/sum final io time and write stencil result matrix to dat file
    GET_TIME(start_io);
    ret = write2D(md.B, md.rows, md.cols, fd.finalfile);
    GET_TIME(end_io);
    *io_time += (end_io-start_io); 
    return ret;
}

int main(int argn, char **argv) {
    int ret = 1; 
    // check if 5 args were entered
    if (argn != 5){
        printf("Usage: %s <num_iterations> <input_date_file> <output_data_file> <all_iterations>\n", argv[0]);
    }else{
         // parse <num iterations> arg as base 10 int
        char * iptr; 
        int num_iterations = strtol((char *)argv[1], &iptr, 10);   
        // check if <num iterations> is valid 
        if(*iptr != '\0' ||   num_iterations < 1){
            printf("Error:  <num iterations> must be a postive non zero integer\n"); 
        }else{
            // initialize doubles to calc times and get start time
            double start_time = 0.0, end_time = 0.0, cpu_time = 0.0, io_time = 0.0, total_time=0.0;
            GET_TIME(start_time);   
            // initialize stencil data struct and file data struct 
            MatrixData md = {NULL, NULL, 0, 0};        
            FileData fd = {argv[2], argv[3], argv[4], TRUE};   
            // check if reading file into md.A was successfu
            if(read2D(&md.A, &md.rows, &md.cols, fd.initfile) == 0){
                // allocate space matrix md.B
                if(malloc1D(&md.B, md.rows, md.cols, "md.B") == 0){
                    init2D(md.B, md.rows, md.cols);          // initialize md.B as a duplicate of md.A
                    printf("Running %d serial stencil iterations...\n", num_iterations);
                    // perfrom stencil iterations
                    ret = stencilAll(md, fd, num_iterations, &io_time);
                    // print final file size if stencilAll is successful
                    if(ret == 0){
                        printf("------------------------------------------------------------------\n");
                        long finalfile_size = ((long)md.rows*(long)md.cols*sizeof(double))+((long)2*(sizeof(int)));
                        printf("Wrote final matrix state to '%s'\n[%s] size = %ld(B) = %.6Lg(GB)\n", fd.finalfile,fd.finalfile, finalfile_size, BtoGB(finalfile_size));
                    }
                    // print all file size if writeRaw file is still enabled
                    if(fd.writeRaw == TRUE){
                        printf("------------------------------------------------------------------\n");
                        long allfile_size = (long)md.rows*(long)md.cols*sizeof(double)*((long) num_iterations+1);
                        printf("Wrote inital matrix state + %d iterations to '%s'\n[%s] size = %ld(B) = %.6Lg(GB)\n", num_iterations, fd.allfile, fd.allfile, allfile_size, BtoGB(allfile_size));
                    }
                    free(md.B);     
                }       
                free(md.A);
            }
            // calculate total time and cpu time, display total times for elapsed, compute, and io
            GET_TIME(end_time);
            total_time = end_time-start_time;
            cpu_time = total_time - io_time;
            printf("------------------------------------------------------------------\n");
            printf("[Elapsed Time] = %g sec\n[I/O Time] = %g sec\n[Compute Time] = %g sec", total_time, io_time, cpu_time);
            printf("\n------------------------------------------------------------------\n");
        }
    }
    exit(ret); 
}