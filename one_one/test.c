#include <stdio.h>
#include "mythread.h"
#include <signal.h>
#include "lock.h"


spinlock sl;

unsigned long c1,c2,c;

void *f1()
{   
    unsigned long limit = 1000000;
    while (limit--)
    {   
        c1++;
        thread_lock(&sl);
        c++;
        thread_unlock(&sl);
    }
}

void *f2()
{
    unsigned long limit = 1000000;
    while (limit--)
    {   
        c2++;
        thread_lock(&sl);
        c++;
        thread_unlock(&sl);

    }
}


int matA[5][5];
int matB[5][5];
int matC[5][5];
int step_i = 0;

void* multi(void* arg)
{
    int i = step_i++; //i denotes row number of resultant matC
   
    for (int j = 0; j < 5; j++)
      for (int k = 0; k < 5; k++)
        matC[i][j] += matA[i][k] * matB[k][j];

}


void timePass(){
    int i = 1000;
    while(i--);
    return;
}

void testingJoin(){
    mythread_t threadArr[21];

    printf("Calling thread_join after all threads are created\n\n");
    for(int i = 0; i < 10; i++){
        if(thread_create(&threadArr[i], NULL, timePass,NULL) == 0){
            printf("Thread with tid %lu created\n", threadArr[i]);
        }else{
            printf("Failed\n");
        }
    }

    for(int i = 0; i < 10; i++){
        thread_join(&threadArr[i],NULL);
    }

    printf("\n\nCalling thread_join after each thread is created\n\n");
    for(int i = 10; i < 20; i++){
        if(thread_create(&threadArr[i], NULL, timePass,NULL) == 0){
            printf("Thread with tid %lu created\n", threadArr[i]);
            thread_join(&threadArr[i], NULL);
        }else{
            printf("Failed\n");
        }
    }


    printf("Join test completed\n");
}


void fun1(void *i){
    int *j = (int*)i;
    printf("\n%d\n",*j);
    return;
}

void fun2(void *i){
    while(1){
        sleep(1);
        printf("doing well\n");
    }
    return;
}

int main(){
    //c1 , c2 test
    // mythread_t t1,t2;
    // initlock(&sl);
    // thread_create(&t1,NULL, f1,NULL);
    // thread_create(&t2,NULL, f2,NULL);
    // sleep(4);
    // printf("c1 = %ld, c2 = %ld\n",c1,c2);
    // printf("c = %ld\n",c);

    //matrix mult
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         matA[i][j] = 1;
    //         matB[i][j] = 1;
    //     }
    // }
    // mythread_t t1,t2,t3,t4,t5;
    // thread_create(&t1,NULL, multi,NULL);
    // thread_create(&t2,NULL, multi,NULL);
    // thread_create(&t3,NULL, multi,NULL);
    // thread_create(&t4,NULL, multi,NULL);
    // thread_create(&t5,NULL, multi,NULL);

    // thread_join(&t1,NULL);
    // thread_join(&t2,NULL);
    // thread_join(&t3,NULL);
    // thread_join(&t4,NULL);
    // thread_join(&t5,NULL);

    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++)
    //         printf("%d ",matC[i][j]);       
    //     printf("\n");
    // }


    //join test
    // testingJoin();
    int *r1 = (int *)malloc(sizeof(int));
    int *r2 = (int *)malloc(sizeof(int));
    int p = 9,q = 10;
    printf("in");
    mythread_t t1,t2;
    thread_create(&t1,NULL, fun1,(void*)&p);
    thread_create(&t2,NULL, fun2,(void*)&q);
    sleep(2);
    thread_kill(&t2, SIGTERM);
    printf("hey");
    sleep(7);
    return 0;

}