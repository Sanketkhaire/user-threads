#ifndef MNMYTHREAD_H
#define MNMYTHREAD_H

#include <time.h>
#include <setjmp.h>
#define GUARDPSIZE (getpagesize())
#define DEFAULT_STACKSIZE (4*getpagesize())

enum threadState{RUNNABLE,TERMINATED,RUNNING,EMBRYO};

typedef unsigned long mythread_t;

typedef struct funcDesc{
    void (*fPtr)(void *);
    void *args;
    void *stack;  
    int status; 
}funcDesc;

typedef struct thDesc{
    mythread_t tid;
    pid_t pid;
    pid_t ppid;
    mythread_t kid;
    int signalArr[50];
    int sigIndex;
    jmp_buf myContext;
    jmp_buf exitPoint;
}thDesc;

//linked list node data structure for storing threads created.
typedef struct node{
    thDesc *th;
    funcDesc *fD;
    struct node *next;
}node;


//actual linked list structure enclosing head and tail pointer.
typedef struct linked_list{
    node *start;
    int count;
}th_linked_list;

typedef struct kthread{
    mythread_t kid;
    mythread_t tid;//tid which is to be wsheduled on 
    void *stack;
    struct kthread *next;
    jmp_buf scheduler_context; 
}kthread;

typedef struct klinked_list{
    kthread *start;
    kthread *current;//this to be scheduled each time 
    int max_kernel_threads;
}kth_linked_list;

void add_thread_to_ll(thDesc *t, funcDesc *f);
void add_kthread_to_ll(kthread *k);
static int wrapper();
void mythread_setkthreads(int no_of_kernel_threads);
int thread_create(mythread_t *t,void *attr, void *func_ptr, void *args);
int thread_join(mythread_t *t, void **retval);
void thread_exit(void *retval);
int thread_kill(mythread_t *T, int sig);

#endif