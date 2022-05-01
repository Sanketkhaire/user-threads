#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "../mythread.h"
#include "../lock.h"
#include "colors.h"

spinlock sl;
unsigned long c1,c2,c3,c;
void *f1(){
    unsigned long limit = 1000;
    while (limit--){
        c1++;
        thread_lock(&sl);
        c++;
        thread_unlock(&sl);
    }
}

void *f2(){
    unsigned long limit = 1000000;
    while (limit--){
        c2++;
        thread_lock(&sl);
        c++;
        thread_unlock(&sl);
    }
}

void *f3(){
    unsigned long limit = 1000000;
    while (limit--){
        c3++;
        thread_lock(&sl);
        c++;
        thread_unlock(&sl);

    }
}

void stop_sync_testing(){
    printf(BBLUE"<===================================many_one Sync Testing Done==============================>\n"NONE);
    return;
}
void start_sync_testing(){
    printf(BBLUE"<===================================Many-One Race Condition:==============================>\n"NONE);
    return;
}

void stop_matrix_testing(){
    printf(BBLUE"<================Many-One Stress Testing Matrix Multiplication of higher dimensions: Finished!===================>\n"NONE);
    return;
}

int main(){
    stop_matrix_testing();
    start_sync_testing();
    printf(BBLUE"\n\ttesting RACE CONDITIONS!\n"NONE);
    printf(BYELLOW"\tTesting c=c1+c2+c3 for 3 threads\n"NONE);
    mythread_t t1,t2,t3;
    initlock(&sl);
    thread_create(&t1,NULL, f1,NULL);
    thread_create(&t2,NULL, f2,NULL);
    thread_create(&t2,NULL, f3,NULL);

    sleep(4);
    printf(WHITE"\tvalue of c1 = %ld, c2 = %ld, c3 = %ld\n"NONE,c1,c2,c3);
    printf(WHITE"\tvalue of c = %ld\n"NONE,c);
    if(c==c1+c2+c3)
        printf(SUCCESS"\tmany_one Race condition Passed!\n"NONE);
    else
        printf(WHITE"\tmany_one Race condition failed!\n\n"NONE);
    return 0;
}


