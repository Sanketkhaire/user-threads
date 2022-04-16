#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "mythread.h"
#include "lock.h"

void thread_create_test();
void thread_join_test();
void matrix_multiplication_test();
void thread_kill_test();
void thread_exit_test();
void stress_test();
void error_flags_test();

struct test_exit{
    int a;
};
void function(){
    char ch = 'a';
    return;
}

void thread_exit_function(void *passed_struct){
    struct test_exit *t=(struct test_exit*)passed_struct;
    t->a=4;
    printf("value of original%d\n",t->a);
    thread_exit(NULL);
    t->a=5;
    return;
}

void thread_sigKillTerm_function(){
    while(1);
}
void thread_sigStopCont_function(){
    while(1){
          printf("Function running!");
          sleep(2);
    }
}
//creating nearly 20 threads using thread crea
//te and join them one by one 
//running simple function 
void thread_create_test(){
    mythread_t threads[50];
    int flag,create_eflag=0;
    for(int i=0;i<50;i++){
        flag = thread_create(&threads[i],NULL,function,NULL);
        if(flag!=0){
            create_eflag++;
            printf("Thread not created!");
        }
        else
            printf("Thread successfully created\n");
    }
    if(create_eflag==0)
        printf("THREAD_CREATE TEST PASSED");
    else
        printf("THREAD CREATE TEST FAILED!");
    return;
}

void thread_join_test(){
    mythread_t threads[50];
    int flag,create_eflag=0,join_eflag=0;
    for(int i=0;i<50;i++){
        flag = thread_create(&threads[i],NULL,function,NULL);
        if(flag!=0){
            create_eflag++;
            printf("Thread not created!");
        }
    }

    //First create 50 threads , join them one by one 
    int *retval = (int *)malloc(sizeof(int));
    for(int i=0;i<50;i++){
        flag = thread_join(&threads[i],(void *)&retval);
        if(flag == 0){
            printf("Thread joined succesfully\n");
        }
        else{
            join_eflag++;
            printf("Thread Not found!Error!\n");
        }
    }
    if(create_eflag==0 && join_eflag==0)
        printf("THREAD_JOIN TEST PASSED");
    else
        printf("THREAD_JOIN TEST FAILED!");
    return;
    //the ek join  test tajaychi ahe 
}

void thread_kill_test_1(){
    printf("Thread executing while(1) function:\n");
    printf("Kill thread using threaD_kill(SIGKILL)");
    //4 thread create one for sigterm , sigkill, sigcont, sigstop;
    mythread_t t[2];
    int flag = thread_create(&t[0],NULL,thread_sigKillTerm_function,NULL);
    int flag1 = thread_create(&t[1],NULL,thread_sigKillTerm_function,NULL);
    int signal_no1=SIGKILL;
    int signal_no2=SIGTERM;
    int retval1=thread_kill(&t[0],signal_no1);
    if(retval1!=0)
        printf("THREAD_KILL_SIGKILL FAILED");
    else
        printf("THREAD_KILL_SIGKILl PASSED!");
    int retval2=thread_kill(&t[1],signal_no2);
    if(retval2!=0)
        printf("THREAD_KILL_SIGTERM FAILED");
    else
        printf("THREAD_KILL_SIGTERM PASSED!");
}

void thread_kill_test_2(){
    printf("Thread executing while(1) function2:\n");
    printf("stop thread exuection after SIGSTOP, continue execution after SIGCONT,and then finally kill thread:");
    //4 thread create one for sigterm , sigkill, sigcont, sigstop;
    mythread_t t;
    int flag = thread_create(&t,NULL,thread_sigStopCont_function,NULL);
    sleep(4);
    //4 times sleep so function will print 2 times function running!
    int eflag = thread_kill(&t,SIGSTOP);
    if(eflag!=0)
        printf("THREAD_KILL_SIGSTOP FAILED");
    printf("Now execution stopped.sleep(for 3 seconds)");
    sleep(3);
    eflag = thread_kill(&t,SIGCONT);
    if(eflag!=0)
        printf("THREAD_KILL_SIGCONT FAILED");
    printf("execution started");
    sleep(4);
    printf("Now thread_kill : SIGKILL\nwhile loop stopped");
    thread_kill(&t,SIGKILL);
    printf("THREAD_KILL_SIGSTOP, SIGCONT PASSED");
    return;
}
//he function nit kar
void thread_exit_test(){
    mythread_t threads[20];
    struct test_exit array[20];
    int flag,create_eflag=0;
    int exit_flag=0;
    for(int i=0;i<20;i++){
        flag = thread_create(&threads[i],NULL,thread_exit_function,(void *)&array[i]);
        if(flag!=0){
            create_eflag++;
            printf("Thread not created!");
            //ithe exit(1) karyache
        }
    }
    if(create_eflag!=0){
        printf("THREAD_CREATE TEST FAILED");
        return;
    }
    for(int i=0;i<20;i++){
        printf("%d is val\n",array[i].a);
        if(array[i].a==5){
            exit_flag++;
            printf("THREAD_EXIT_TEST_FAILED");
        }
    }
    if(exit_flag!=0)
        printf("THREAD_EXIT_TEST_FAILED");
    else
        printf("THREAD_EXIT_TEST_PASSED");
    return;


}

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

void thread_lock_test(){
    mythread_t t1,t2;
    initlock(&sl);
    thread_create(&t1,NULL, f1,NULL);
    thread_create(&t2,NULL, f2,NULL);
    sleep(4);
    printf("c1 = %ld, c2 = %ld\n",c1,c2);
    printf("c = %ld\n",c);
    return;
}
int main(){
    //thread_create_test();
    //thread_join_test();
    //thread_exit_test();
    thread_lock_test();
    //thread_kill_test_2();
    //thread_kill_test();
    //thread_matrix_multiplication_test();

    return 0;
}
