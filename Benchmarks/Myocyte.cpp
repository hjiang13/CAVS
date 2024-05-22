#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include "define.c"
#include "ecc.c"
#include "cam.c"
#include "fin.c"
#include "master.c"
#include "embedded_fehlberg_7_8.c"
#include "solver.c"
#include "file.c"
#include "timer.c"
//====================================================================================================100
//	MAIN FUNCTION
//====================================================================================================100
int main(int argc, char *argv[]) {
    //================================================================================80
    //		VARIABLES
    //================================================================================80
    //============================================================60
    //		TIME
    //============================================================60
    long long time0;
    long long time1;
    long long time2;
    long long time3;
    long long time4;
    long long time5;
    time0 = get_time();
    //============================================================60
    //		COUNTERS
    //============================================================60
    long long memory;
    int i, j;
    int status;
    int mode;
    //============================================================60
    //		SOLVER PARAMETERS
    //============================================================60
    long workload;
    long xmin;
    long xmax;
    fp h;
    fp tolerance;
    //============================================================60
    //		DATA
    //============================================================60
    fp ***y;
    fp **x;
    fp **params;
    //============================================================60
    //		OPENMP
    //============================================================60
    int threads;
    //================================================================================80
    // 	GET INPUT PARAMETERS
    //================================================================================80
    //============================================================60
    //		CHECK NUMBER OF ARGUMENTS
    //============================================================60
    if (argc != 5) {
        printf("ERROR: %d is the incorrect number of arguments, the number of "
               "arguments must be 4\n",
               argc - 1);
        return 0;
    }
    //============================================================60
    //		GET AND CHECK PARTICULAR ARGUMENTS
    //============================================================60
    else {
        //========================================40
        //		SPAN
        //========================================40
        xmax = atoi(argv[1]);
        if (xmax < 0) {
            printf("ERROR: %d is the incorrect end of simulation interval, use "
                   "numbers > 0\n",
                   xmax);
            return 0;
        }
        //========================================40
        //		WORKLOAD
        //========================================40
        workload = atoi(argv[2]);
        if (workload < 0) {
            printf("ERROR: %d is the incorrect number of instances of "
                   "simulation, use numbers > 0\n",
                   workload);
            return 0;
        }
        //========================================40
        //		MODE
        //========================================40
        mode = 0;
        mode = atoi(argv[3]);
        if (mode != 0 && mode != 1) {
            printf("ERROR: %d is the incorrect mode, it should be omitted or "
                   "equal to 0 or 1\n",
                   mode);
            return 0;
        }
        //========================================40
        //		THREADS
        //========================================40
        threads = atoi(argv[4]);
        if (threads < 0) {
            printf("ERROR: %d is the incorrect number of threads, use numbers "
                   "> 0\n",
                   threads);
            return 0;
        }
        omp_set_num_threads(threads);
    }
    time1 = get_time();
    //================================================================================80
    // 	ALLOCATE MEMORY
    //================================================================================80
    //============================================================60
    //		MEMORY CHECK
    //============================================================60
    memory = workload * (xmax + 1) * EQUATIONS * 4;
    if (memory > 1000000000) {
        printf("ERROR: trying to allocate more than 1.0GB of memory, decrease "
               "workload and span parameters or change memory parameter\n");
        return 0;
    }
    //============================================================60
    // 	ALLOCATE ARRAYS
    //============================================================60
    y = (fp ***)malloc(workload * sizeof(fp **));
    for (i = 0; i < workload; i++) {
        y[i] = (fp **)malloc((1 + xmax) * sizeof(fp *));
        for (j = 0; j < (1 + xmax); j++) {
            y[i][j] = (fp *)malloc(EQUATIONS * sizeof(fp));
        }
    }
    x = (fp **)malloc(workload * sizeof(fp *));
    for (i = 0; i < workload; i++) {
        x[i] = (fp *)malloc((1 + xmax) * sizeof(fp));
    }
    params = (fp **)malloc(workload * sizeof(fp *));
    for (i = 0; i < workload; i++) {
        params[i] = (fp *)malloc(PARAMETERS * sizeof(fp));
    }
    time2 = get_time();
    //================================================================================80
    // 	INITIAL VALUES
    //================================================================================80
    // y
    for (i = 0; i < workload; i++) {
        read("../../data/myocyte/y.txt", y[i][0], 91, 1, 0);
    }
    // params
    for (i = 0; i < workload; i++) {
        read("../../data/myocyte/params.txt", params[i], 16, 1, 0);
    }
    time3 = get_time();
    //================================================================================80
    //	EXECUTION
    //================================================================================80
    if (mode == 0) {
        for (i = 0; i < workload; i++) {
            status = solver(y[i], x[i], xmax, params[i], mode);
            // if(status !=0){
            // printf("STATUS: %d\n", status);
            // }
        }
    } else {
#pragma omp parallel for private(i, status) shared(y, x, xmax, params, mode)
        for (i = 0; i < workload; i++) {
            status = solver(y[i], x[i], xmax, params[i], mode);
            // if(status !=0){
            // printf("STATUS: %d\n", status);
            // }
        }
    }
    // // print results
    // int k;
    // for(i=0; i<workload; i++){
    // printf("WORKLOAD %d:\n", i);
    // for(j=0; j<(xmax+1); j++){
    // printf("\tTIME %d:\n", j);
    // for(k=0; k<EQUATIONS; k++){
    // printf("\t\ty[%d][%d][%d]=%13.10f\n", i, j, k, y[i][j][k]);
    // }
    // }
    // }
    time4 = get_time();
    //================================================================================80
    //	DEALLOCATION
    //================================================================================80
    // y values
    for (i = 0; i < workload; i++) {
        for (j = 0; j < (1 + xmax); j++) {
            free(y[i][j]);
        }
        free(y[i]);
    }
    free(y);
    // x values
    for (i = 0; i < workload; i++) {
        free(x[i]);
    }
    free(x);
    // parameters
    for (i = 0; i < workload; i++) {
        free(params[i]);
    }
    free(params);
    time5 = get_time();
    //================================================================================80
    //		DISPLAY TIMING
    //================================================================================80
    printf("Time spent in different stages of the application:\n");
    printf("%.12f s, %.12f % : SETUP VARIABLES, READ COMMAND LINE ARGUMENTS\n",
           (float)(time1 - time0) / 1000000,
           (float)(time1 - time0) / (float)(time5 - time0) * 100);
    printf("%.12f s, %.12f % : ALLOCATE MEMORY\n",
           (float)(time2 - time1) / 1000000,
           (float)(time2 - time1) / (float)(time5 - time0) * 100);
    printf("%.12f s, %.12f % : READ DATA FROM FILES\n",
           (float)(time3 - time2) / 1000000,
           (float)(time3 - time2) / (float)(time5 - time0) * 100);
    printf("%.12f s, %.12f % : RUN COMPUTATION\n",
           (float)(time4 - time3) / 1000000,
           (float)(time4 - time3) / (float)(time5 - time0) * 100);
    printf("%.12f s, %.12f % : FREE MEMORY\n", (float)(time5 - time4) / 1000000,
           (float)(time5 - time4) / (float)(time5 - time0) * 100);
    printf("Total time:\n");
    printf("%.12f s\n", (float)(time5 - time0) / 1000000);
    //====================================================================================================100
    //	END OF FILE
    //====================================================================================================100
}


