#define GUARDPSIZE (getpagesize())
#define DEFAULT_STACKSIZE (16*getpagesize())
#include <setjmp.h>

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


void add_thread_to_ll(node* n,thDesc *t, funcDesc *f);
int wrapper(void *arg);
int thread_create(mythread_t *t,void *attr, void *func_ptr, void *args);
int thread_join(mythread_t *t, void **retval);
void thread_exit(void *retval);
int thread_kill(mythread_t *T, int sig);

