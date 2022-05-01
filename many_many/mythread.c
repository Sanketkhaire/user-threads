#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/sched.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <limits.h>
#include<limits.h>
#include <setjmp.h>
#include "mythread.h"
#include "lock.h"

th_linked_list thread_chain;
kth_linked_list kthread_chain={NULL,NULL,3};
spinlock sl;



// extern enum threadState state;

sigset_t __signalList;


static int wrapper(){
    int *s = (int*)malloc(sizeof(int));
    printf("In Wrapper!");
    node *curr = thread_chain.start;
    int ktid = gettid();

    acquire(&sl);
    while(curr){
        if(curr->fD->status == RUNNING && curr->th->kid == ktid){
            printf("\n  found  \n");
            break;
        }   
        curr = curr->next;
    }
    release(&sl);
    printf("In wrapper by gettid:%d\n",ktid);
    if(curr!=NULL){
        printf("\n  scheduled  \n");
        funcDesc *f =(funcDesc *)curr->fD;
        f->fPtr(f->args);
        thread_exit(NULL);
    }
    // else
    //     printf("HH");
    //thread_exit((void*)s);
    // while(1){
    //     sleep(1);
    // }
   
    printf("scheduled down\n");
    kthread *start = kthread_chain.start;
    // exit(1);
    while(start){
        if(start->kid == ktid)
            break;
        else
            start=start->next;
    }
    // if(!start){ 
    //     printf("lkkhgffds");
    //     return 0;
    // }
    longjmp(start->scheduler_context,1);
	return 0;
}

void traverse(){
    printf("lll\n");
    node *n = thread_chain.start;
    while(n){
        printf("%ld is tid\n",n->th->tid);
        n=n->next;
    }
}

//Ref:https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/sigpmsk.htm
static void setSignals()
{
    
    sigemptyset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
    sigaddset(&__signalList, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &__signalList, NULL);
    return;
}

void add_thread_to_ll(thDesc *t, funcDesc *f){
    node *newNode = (node *)malloc(sizeof(node));
    newNode->th = t;
    newNode->fD = f;
    newNode->next = NULL;
    newNode->th->tid = thread_chain.count;
    (thread_chain.count)++;
    newNode->next = thread_chain.start;
    thread_chain.start=newNode;
    return;
}

void add_kthread_to_ll(kthread *k){
    if(kthread_chain.start== NULL){
        kthread_chain.start = k;
    }
    else{
        //printf("last\n");
        k->next=kthread_chain.start;
        kthread_chain.start=k;
    }
    return;
}
void sig_handler2(){
    ualarm(0,0);
    int ktid = gettid();
    printf("Now control transferred to thread with tid:%ld and cactual ktid:%d\n",kthread_chain.current->kid,ktid);
    node *start = thread_chain.start;
    
    while(start){
        printf("dowiopppp\n");
        if(start->fD->status == RUNNING && start->th->kid==ktid){
            printf("down\n");
            // start->fD->status = RUNNABLE;
            break;
        }
        start=start->next;
    }
    printf("corr\n");
    if(start!=NULL){
        printf("gfg");
                // exit(1);
        if(setjmp(start->th->myContext)==0)
            longjmp(kthread_chain.current->scheduler_context,1);
    }
    else{
        
        printf("else\n");
        // exit(1);
        longjmp(kthread_chain.current->scheduler_context,1);
        //longjmp(scheduler_context_global,2);
    }
    printf("uyttr");
}

void sig_handler(){
    ualarm(0,0);
    printf("In sig handler:%d\n",gettid());
    if(kthread_chain.current==NULL){
        kthread *first_thread = kthread_chain.start;
        kthread_chain.current = first_thread;
        int k = tgkill(getpid(),kthread_chain.current->kid,SIGUSR1);
        printf("%d is k",k);
    }
    else{
        if(kthread_chain.current->next==NULL){
            kthread_chain.current=kthread_chain.start;
            tgkill(getpid(),kthread_chain.current->kid,SIGUSR1);
        }
        else{
            kthread *temp=kthread_chain.current;
            temp=temp->next;
            kthread_chain.current=temp;
            tgkill(getpid(),kthread_chain.current->kid,SIGUSR1);
        }
    }
    return;
}

int scheduler(void *arg){
    //setSignals();
    setSignals();
    printf("scheduler started for one of kernel threads with kid:%d\n",gettid());
    //signal(SIGALRM,signal_handler);
    /*Ref:https://stackoverflow.com/questions/16826898/error-struct-sigevent-has-no-member-named-sigev-notify-thread-id*/
    /*https://linux.kernel.narkive.com/nVHaTOMT/setitimer-vs-threads-sigalrm-returned-to-which-thread-process-master-or-individual-child*/
    int ktid = gettid();
    kthread *start = kthread_chain.start;
    while(start){
        if(start->kid == ktid)
            break;
        else
            start=start->next;
    }
    node *current = thread_chain.start;
    while(current){
        acquire(&sl);
        node *temp = current;
        current = current->next;
        if(!current) current = thread_chain.start;
        //traverse();
        //printf("dsjkfh");
        if(temp->fD->status == RUNNABLE){
            printf("Runnable thread Found!on scheduler running on kid:%d",gettid());
            temp->th->kid = ktid;
            start->tid = temp->th->tid;
            release(&sl);
            temp->fD->status = RUNNING;
            printf("setting ualarm(100,0)\n");
            ualarm(100,0);
            if(setjmp(start->scheduler_context)==0)
                longjmp(temp->th->myContext,1);
            printf("came here after long jmp to scheduler\n");
            if(temp->fD->status != TERMINATED)
                temp->fD->status = RUNNABLE;
            temp->th->kid = 0;
            acquire(&sl);
        }
        release(&sl);
    }
    return 0;
}

/*Ref:https://stackoverflow.com/questions/69148708/alternative-to-mangling-jmp-buf-in-c-for-a-context-switch*/
unsigned long int mangle(unsigned long int p) {
    unsigned long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
        : "=r"(ret)
        : "r"(p)
        : "%rax"
    );
    return ret;
}

void mythread_setkthreads(int no_of_kthreads){
    kthread_chain.max_kernel_threads = no_of_kthreads;
    return;
}

int thread_create(mythread_t *tt,void *attr, void *func_ptr, void *arg){
    static int kthread_no = 1;
    static int flag = 0;
    if(flag==0){
        flag=1;
        initlock(&sl);
        signal(SIGALRM,sig_handler);
        signal(SIGUSR1,sig_handler2);
    }
    if(kthread_no <= 3){
        printf("kernel Thread created in thread_create\n");
        void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
        if(new_stack == MAP_FAILED)
             printf("|mmap failed!|\n");
        kthread *k = (kthread *)malloc(sizeof(kthread));
        k->stack = new_stack;
        
        int id = clone(scheduler, new_stack+GUARDPSIZE+DEFAULT_STACKSIZE,CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD |CLONE_SYSVSEM|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, NULL);
        if(id == -1){
            printf("|Clone Failed!|\n");
            //adding error flag here 
            exit(1);
        } 
        add_kthread_to_ll(k);
        printf("Returned after Clone in thread_Create!");
        k->kid = id;
        kthread_no++;
        //userthread creatiowhile(1){
    // //    printf("Hello in f:");
    //     sleep(2);
    // }n
    }
    printf("User thread created now!\n");
    void *u_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    thDesc *t = (thDesc *)malloc(sizeof(thDesc));
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = arg;
    f->stack = u_stack;
    t->myContext->__jmpbuf[6] = mangle((long int)(u_stack+GUARDPSIZE+DEFAULT_STACKSIZE));
    t->myContext->__jmpbuf[7] = mangle((long int)wrapper);
    /*
    t->sigIndex = 0;
    for(int j = 0; j < 50; j++){
        t->signalArr[j] = 0;
    }
    */
    t->pid = getpid();
    t->ppid = getppid();
    add_thread_to_ll(t, f);
    printf("Thread addded to linked list and marked as runnable\n");
    *tt = t->tid;
    f->status = RUNNABLE;
    return 0;
}

int thread_join(mythread_t *t, void **retval){  
    node *temp = thread_chain.start;
    
    acquire(&sl);
    while(temp){
        if(temp->th->tid == *t){
            printf("\nkids : %ld\n",temp->th->tid);
            break;
        }
        temp = temp->next;
    }
    release(&sl);

    if(temp){
        printf("\nstatus : %d\n",temp->fD->status);
        while(temp->fD->status != TERMINATED)
            ;
        return 0;
    }
    else{
        printf("not found!");
        return -1;
    }

}



void thread_exit(void *retval){
    
    node *temp = thread_chain.start;
    int ktid = gettid();

    acquire(&sl);
    while(temp){
        if(temp->fD->status = RUNNING && temp->th->kid == ktid){
            temp->fD->status = TERMINATED;
            break;
        }
        temp=temp->next;
    }
    release(&sl);
    
    return;
}



int thread_kill(mythread_t *t, int sig){
    node *temp = thread_chain.start;

    while(temp){
        if(temp->th->tid == *t){
            break;
        }
        temp=temp->next;
    }

    if(temp){

        if(temp->fD->status == RUNNING){
            if(sig == SIGKILL || sig == SIGCONT || sig == SIGSTOP || sig == SIGINT){
                int m = tgkill(temp->th->pid,temp->th->kid,sig);
                if(m == -1)
                    printf("Not killed!");
            }
            else if(sig == SIGTERM){
                thread_exit(NULL);
            }
        }
        else if(temp->fD->status != TERMINATED){    
            if(sig == SIGKILL || sig == SIGCONT || sig == SIGSTOP || sig == SIGINT){
                int m = tgkill(temp->th->pid,temp->th->kid,sig);
                if(m == -1)
                    printf("Not killed!");
            }
            else if(sig == SIGTERM){
                acquire(&sl);
                temp->fD->status = TERMINATED;
                release(&sl);
            }
            else{
                temp->th->signalArr[temp->th->sigIndex++] = sig;
            }
        }
    }
}

/*

//     node *n = thread_chain.start->next;
//     pid_t pid=getpid();
//     while(n){
//         if(n->th->
//         n=n->next;
//     }
//     if(sig == SIGKILL || sig == SIGTERM){
//         int m= tgkill(pid,*t,sig);
//         if(m==-1)
//             printf("Not killed!");
//     }
//     else if(sig == SIGSTOP){
//         int m= tgkill(pid,*t,sig);
//         if(m==-1)
//             printf("Not Stopped!");

//     }
//     else if(sig == SIGCONT){
//          int m= tgkill(pid,*t,sig);
//         if(m==-1)
//             printf("Not Continued!");

//     }

// }
*/

struct c{
    int a,b,result;
};
void f(){
    printf("Sankettt\n");
    while(1){
        printf("Hello in f:");
       sleep(2);
    }
    //printf("%d is :",cd->result);
    return;
}

void g(){
    printf("5 secs completed!\n");
    //sleep(2);
    printf("h-----\n");
    return;
}

int main(){

    printf("Program started in Main: %d\n",gettid());
    thread_chain.start = NULL;
    thread_chain.count = 0;
    int *ret = (int *)malloc(sizeof(int));
    void** re;
    mythread_t t,k,m,n;
    thread_create(&t,NULL, g,NULL);
    printf("\nthread in Main()");
    thread_create(&k,NULL, f,NULL);
    thread_create(&m,NULL, f,NULL);
    thread_create(&n,NULL, f,NULL);
    //thread_create(&m,NULL, f,NULL);
    printf("thread create done\n");
    sleep(5);
    // thread_join(&t,re);
    /*sleep(2);
    thread_killt,SIGSTOP);
    printf("No");
    thread_kill(&t,SIGCONT);
    sleep(4);(&
    printf("\n%d is ",sanket.result);*/
    //thread_kill(&t, SIGKILL);
    printf("i am out\n");
   // sleep(5);
    while(1);
    // printf("i poiu\n");
    // sleep(10);
    // }
    return 0;
}
