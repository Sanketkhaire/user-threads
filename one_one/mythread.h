#define GUARDPSIZE (getpagesize())
#define DEFAULT_STACKSIZE (4*getpagesize())

typedef unsigned long mythread_t;

typedef struct funcDesc{
    void (*fPtr)(void *);
    void *args;
    void *stack;  
    int status; 
}funcDesc;

typedef struct thread{
    mythread_t tid;
    mythread_t kid;
}mythread;

//linked list node data structure for storing threads created.
typedef struct node{
    mythread *th;
    funcDesc *fD;
    struct node *next;
}node;


//actual linked list structure enclosing head and tail pointer.
struct linked_list{
    node *start;
    node *end;
    int count;
};

typedef struct linked_list thread_ll;

void add_thread_to_ll(thread_ll *list, mythread *t, funcDesc *f);
static int wrapper(void *arg);
int thread_create(mythread *t,void *attr, void *func_ptr, void *args);
int thread_join(mythread *t, void **retval);
void thread_exit(void *retval);
int thread_kill(mythread *T, int sig);

