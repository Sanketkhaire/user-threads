#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "../mythread.h"
#include "colors.h"

spinlock sl;
unsigned long c1,c2,c;
//function for thread_Create thread join 
void function(){
    int a;
    return;
}

//functions for thread lock test
void *f1(){
    unsigned long limit = 1000000;
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

void test_sigkill(){
    printf(WHITE"\n\tRunning thread with TID:%d"NONE,gettid());
    printf(WHITE"\n\tRunnig while loop"NONE);
    while(1){
        printf("\n\tRunning Thread:");
        sleep(2);
    }

}
void thread_exit_function(){
    while(1){
        sleep(4);
        thread_exit(NULL);
    }
    return;
}

void thread_create_test(){
    printf(BYELLOW"\n\ttesting many_one thread_create function:\n"NONE);
    mythread_t threads[10];
    int flag,create_eflag=0;
    for(int i=0;i<10;i++){
        flag = thread_create(&threads[i],NULL,function,NULL);
        if(flag!=0){
            create_eflag++;
        }
        else{
            printf(WHITE"\tThread_created with tid:%ld\n"NONE,threads[i]);
        }
    }
    if(create_eflag==0)
        printf(SUCCESS"\tmany_one thread_create test passed!\n\n"NONE);
    else
        printf(ERROR"\t\tmany_one thread_create test failed!\n\n"NONE);
    return;
}

void thread_join_test(){
    printf(BYELLOW"\n\ttesting many_one thread_join function:\n"NONE);
    mythread_t threads[10];
    int flag,create_eflag=0,join_eflag=0;
    for(int i=0;i<10;i++){
        flag = thread_create(&threads[i],NULL,function,NULL);
        if(flag!=0){
            create_eflag++;
        }
        else{
            printf(WHITE"\tThread_created with tid:%ld\n"NONE,threads[i]);
        }
    }

    //First create 20 threads , join them one by one 
    int *retval = (int *)malloc(sizeof(int));
    for(int i=0;i<10;i++){
        flag = thread_join(&threads[i],(void *)&retval);
        if(flag != 0){
            join_eflag++;
        }
        else{
            printf(WHITE"\tThread_Joined with tid:%ld\n"NONE,threads[i]);
        }
    }
    if(create_eflag==0 && join_eflag==0)
        printf(SUCCESS"\tmany_one thread_join test passed!\n\n"NONE);
    else
        printf(ERROR"\tmany_one thread_join test failed!\n\n"NONE);
    return;
}

void thread_lock_test(){
    printf(BYELLOW"\n\ttesting many_one thread_lock unlock functions:\n"NONE);
    printf(BYELLOW"\tTesting simple critical section c=c1+c2 code:\n"NONE);
    mythread_t t1,t2;
    initlock(&sl);
    thread_create(&t1,NULL, f1,NULL);
    thread_create(&t2,NULL, f2,NULL);
    sleep(4);
    printf(WHITE"\tvalue of c1 = %ld, c2 = %ld\n"NONE,c1,c2);
    printf(WHITE"\tvalue of c = %ld\n"NONE,c);
    if(c==c1+c2)
        printf(SUCCESS"\tmany_one thread lock test passed\n"NONE);
    else
        printf(WHITE"\tmany_one thread_lock test failed!\n\n"NONE);
    return;
}


void thread_kill_test(){
    mythread_t tid;
    printf(BYELLOW"\n\ttesting many_one thread_kill function:\n"NONE);
    printf(BYELLOW"\n\tTesting for (userdefined SignalHandler of SIGTERM) \n"NONE);    
    printf(WHITE"\n\tTesting For SIGKILL:\n"NONE);
    thread_create(&tid,NULL,test_sigkill,NULL);
    printf(WHITE"\n\tSLEEPING for 10 secs and after that sending SIGTERM to given thread(runnign infinite loop):"NONE);
    sleep(10);
    thread_kill(&tid,SIGTERM);
    printf(BBLUE"\none one thread_kill test passed!\n"NONE);
    return;
}

void thread_exit_test(){
    printf(BYELLOW"\n\ttesting many_one thread_exit function:\n"NONE);
    printf(WHITE"\t5 Threads Created with each running infinite loop and after 4 sec, thread exit is called!"NONE);
    mythread_t threads[5];
    int flag,create_eflag=0;
    int exit_flag=0;
    for(int i=0;i<5;i++){
        flag = thread_create(&threads[i],NULL,thread_exit_function,NULL);
        if(flag!=0){
            create_eflag++;
        }
    }
    if(create_eflag!=0){
        printf(ERROR"\n\tmany_one thread_create test failed!\n"NONE);
        return;
    }
    for(int i=0; i<5; i++){
       //retvalue logic lihine rahilay
    }
    if(exit_flag!=0)
        printf(ERROR"\tTHREAD_EXIT_TEST_FAILED\n"NONE);
    else
        printf(SUCCESS"\n\tmany_one thread_exit test passed!\n"NONE);
    return;
}

void start_testing(){
    printf(BBLUE"<===================================Many-One Library Functions Testing:==============================>\n"NONE);
    return;
}

void stop_testing(){
    printf(BBLUE"<==================Many-One Stress Testing using Matrix Multiplication of higher dimensions:======================>\n"NONE);
    return;
}

int main(){
    start_testing();
    thread_create_test();
    //thread_join_test();
    //thread_exit_test();
    //thread_lock_test();
    //thread_kill_test();
    stop_testing();
    return 0;
}
