#define GUARDPSIZE (getpagesize())
#define DEFAULT_STACKSIZE (4*getpagesize())


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
    int sigIndex = 0;
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
    node *end;
    int count;
}th_linked_list;


void add_thread_to_ll(thDesc *t, funcDesc *f);
static int wrapper();
int thread_create(mythread_t *t,void *attr, void *func_ptr, void *args);
int thread_join(mythread_t *t, void **retval);
void thread_exit(void *retval);
int thread_kill(mythread_t *T, int sig);

