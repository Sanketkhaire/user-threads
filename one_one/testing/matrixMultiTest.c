#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "colors.h"
#include "../mythread.h"

//testing to be done for 30, 50,80,100,1000
int current_itr=0, ndim;

int **matrixA, **matrixB;
int **matrixC;

void populate_matrices(){
    matrixA=(int **)malloc(sizeof(int *)*ndim);
    matrixB=(int **)malloc(sizeof(int *)*ndim);
    matrixC=(int **)malloc(sizeof(int *)*ndim);
    for(int i = 0; i < ndim; i++){
        matrixA[i] =(int *)malloc(sizeof(int)*ndim);
        matrixB[i] =(int *)malloc(sizeof(int)*ndim);
        matrixC[i] =(int *)malloc(sizeof(int)*ndim);
        for(int j = 0; j < ndim; j++){
            matrixA[i][j] = rand()%6;
            matrixB[i][j] = rand()%10;
            matrixC[i][j] = 0;
        }
    }
    return;
}

double my_clock(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}

void fill_matrixC_with_zero(){
    for(int i = 0; i < ndim; i++){
        for(int j = 0; j <ndim; j++){
            matrixC[i][j] = 0;
        }
    }
    return;
}

void matrix_mul_without_threads(){
   for (int i = 0; i < ndim; i++) {
       for (int j = 0; j < ndim; j++){ 
           for (int k = 0; k < ndim; k++) {
               matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
           }
       }
   }
   return; 
}


void *matrix_th(void *args){
    int i = current_itr++; 
    for (int j = 0; j < ndim; j++)
      for (int k = 0; k < ndim; k++)
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
}

void matrix_mul_with_threads(){
    mythread_t threads[ndim];
    int *retval = (int *)malloc(sizeof(int));
    for(int i=0;i<ndim;i++){
        thread_create(&threads[i],NULL,matrix_th,NULL);
    }
    for(int i=0;i<ndim;i++){
        thread_join(&threads[i],(void *)&retval);
    }
    return;
}

int main(int argc, char *argv[]){
    ndim =  atoi(argv[1]);
    //srand(time(0));
    populate_matrices();
    double start_time, end_time;
    printf(BYELLOW"\tMatrix multiplication(Dimension): %d * %d \n"NONE, ndim, ndim);
    start_time = my_clock();
    matrix_mul_with_threads();
    end_time = my_clock();
    printf(SUCCESS"\tTime Required with MultiThreading:%lf seconds\n"NONE, end_time-start_time);

    fill_matrixC_with_zero();
    start_time = my_clock();
    matrix_mul_without_threads();
    end_time = my_clock();
    printf(SUCCESS"\tTime Required withOut MultiThreading:%lf seconds\n"NONE, end_time-start_time);
}
