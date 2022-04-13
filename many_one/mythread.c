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
#include <setjmp.h>
#include "mythread.h"


th_linked_list thread_chain;

enum threadState{RUNNABLE,TERMINATED,RUNNING,EMBRYO,WAITING};

sigset_t __signalList;

static int wrapper(){
    int *s = (int*)malloc(sizeof(int));
    //printf("In Wrapper!");
    node *curr = thread_chain.start->next;
    while(curr){
        if(curr->fD->status == RUNNING){
            break;
        }
            
        curr = curr->next;
    }
    funcDesc *f =(funcDesc *)curr->fD;
    f->fPtr(f->args);
    thread_exit((void*)s);
    longjmp(thread_chain.start->th->myContext,1);
	return 0;
}

void traverse(){
    node *n = thread_chain.start;
    for(int i=0;i<thread_chain.count;i++){
        printf("%ld is tid status : %d\n",n->th->tid,n->fD->status);
        n = n->next;
    }
}

//Ref:https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/sigpmsk.htm
static void setSignals()
{
    
    sigfillset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
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
    if(thread_chain.start && thread_chain.start == thread_chain.end){
        printf("poiu22\n");
        newNode->next = thread_chain.start;
        thread_chain.start = newNode;
        newNode->th->tid = INT_MAX;
        return;
     }
     if(thread_chain.start && thread_chain.start->next == thread_chain.end){
         printf("poiu\n");
         newNode->next = thread_chain.end;
         thread_chain.start->next = newNode;
         return;
     }
    if(thread_chain.start== NULL){
        thread_chain.start = newNode;
        thread_chain.end = newNode;
        printf("printed %ld\n",thread_chain.start->th->kid);
    }
    else{
        printf("last\n");
        thread_chain.end->next = newNode;
    }
    return;
}

void sig_handler(){
    
    printf("in handler\n");
    
    node *curr = thread_chain.start->next;
    while(curr){
        if(curr->fD->status == RUNNING){
            printf("in handler %ld\n",curr->th->tid);
            curr->fD->status = RUNNABLE;
            break;
        }
        curr = curr->next;
        if(!curr) curr = thread_chain.start->next;
    }
    printf("\ninnnnnnnnnn\n");
    if(setjmp(curr->th->myContext) == 0)
        longjmp(thread_chain.start->th->myContext,1);
    
};

void my_signal_handler(node *curr){
    int temp = 0;
    printf("in mysig\n");
    while(temp < curr->th->sigIndex){
        if(curr->th->signalArr[temp] == SIGCONT)
            curr->fD->status = RUNNABLE;
        else if(curr->th->signalArr[temp] == SIGSTOP)
            curr->fD->status = WAITING;
        printf("in array %d\n",curr->th->sigIndex);
        temp++;
    }
    curr->th->sigIndex = 0;
    return;
}

void scheduler(){
    //setSignals();
    node *current = thread_chain.start->next->next, *temp = NULL;
    if((setjmp(thread_chain.start->th->myContext)) == 0){
        setSignals();
        ualarm(100,0);
        thread_chain.start->next->fD->status = RUNNING;
        longjmp(thread_chain.start->next->th->myContext,3);
    }

    while(current){
        temp = current;
        current = current->next;
        // printf("thread no issssss:%ld\n",temp->th->tid);
        if(!current) current = thread_chain.start->next;
        // traverse();
        my_signal_handler(temp);
        if(temp->fD->status == RUNNABLE){
            printf("thread no :%ld\n",temp->th->tid);
            temp->fD->status = RUNNING;
            setSignals();
            ualarm(100,0);
            
            longjmp(temp->th->myContext,3);
        }

    }
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


void init(){
    static int flag = 0;
    if(!flag){
        printf("init\n");
        flag = 1;
        mythread_t t1,t2;
        thread_create(&t1,NULL,scheduler,NULL);
        thread_chain.start->th->myContext->__jmpbuf[7] = mangle((long int)scheduler);
        signal(SIGALRM,sig_handler);
        thread_create(&t2,NULL,NULL,NULL);
        if((setjmp(thread_chain.start->next->th->myContext)) == 0){
            longjmp(thread_chain.start->th->myContext,3);
            //scheduler();
        }
    }
}

int thread_create(mythread_t *tt,void *attr, void *func_ptr, void *arg){

    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);

    thDesc *t = (thDesc *)malloc(sizeof(thDesc));
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = arg;
    f->stack = new_stack;
    f->status = RUNNABLE;
    printf("hey\n");
    //Ref: Coroutines.pdf already pushed.
    t->myContext->__jmpbuf[6] = mangle((long int)(new_stack+GUARDPSIZE+DEFAULT_STACKSIZE));
    t->myContext->__jmpbuf[7] = mangle((long int)wrapper);
    t->sigIndex = 0;
    for(int j = 0; j < 50; j++){
        t->signalArr[j] = 0;
    }
    t->pid = getpid();
    t->ppid = getppid();
    t->kid = getpid();  
    add_thread_to_ll(t, f);
    
    *tt = t->tid;
    init();
    return 0;
}

int thread_join(mythread_t *t, void **retval){  
    node *temp = thread_chain.start->next;
    while(temp){
        if(temp->th->tid == *t){
            printf("\nkids : %ld\n",temp->th->tid);
            break;
        }
        temp = temp->next;
    }
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
    node *temp = thread_chain.start->next;
    while(temp){
        if(temp->fD->status == RUNNING){
            printf("\nexiteddd : %ld\n",temp->th->tid);
            temp->fD->status = TERMINATED;
            break;
        }
        temp=temp->next;
    }
   
    return;
}


int thread_kill(mythread_t *t, int sig){
    node *temp = thread_chain.start->next;
    printf("\nin loopsss\n");
    while(temp){
        printf("\nin loop\n");
        if(temp->th->tid == *t){
            break;
        }
        temp=temp->next;
    }
    printf("in kelll\n");
    if(temp){
        // if(temp->fD->status == RUNNING){
        //     if(sig == SIGKILL || sig == SIGTERM){
        //         int m = tgkill(temp->th->pid,*t,sig);
        //         if(m == -1)
        //             printf("Not killed!");
        //     }
        //     else if(sig == SIGSTOP){
        //         temp->fD->status = WAITING;
        //         if(setjmp(temp->th->myContext) == 0)
        //             longjmp(thread_chain.start->th->myContext,3);
                
        //     }
        //     // else if(sig == SIGCONT){
        //     //     temp->fD->status = RUNNABLE;
        //     // }
        // }

        if(temp->fD->status != TERMINATED){
            
            if(sig == SIGKILL || sig == SIGTERM){
                int m = tgkill(temp->th->pid,*t,sig);
                if(m == -1)
                    printf("Not killed!");
            }
            else if(sig == SIGCONT || sig == SIGSTOP){
                temp->th->signalArr[temp->th->sigIndex++] = sig;
            }
        }
    }
}


struct c{
    int a,b,result;
};

void f(){
    int i = 20;
    while(i--){
        printf("Hello in f:\n");
        sleep(2);
    }
    printf("f completed\n");
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
    thread_chain.start = NULL;
    thread_chain.end = NULL;
    thread_chain.count = 0;
    int *ret = (int *)malloc(sizeof(int));
    void** re;
    mythread_t t,k;
    thread_create(&t,NULL, f,NULL);
    thread_create(&k,NULL, g,NULL);
    printf("jjjjjppp");
    //thread_join(&t,re);
    sleep(2);
    thread_kill(&t,SIGSTOP);
    printf("hey");
    
    sleep(10);
    // exit(1);
    thread_kill(&t,SIGCONT);    
    /*sleep(2);
    thread_kill(&t,SIGSTOP);
    printf("No");
    thread_kill(&t,SIGCONT);
    sleep(4);
    printf("\n%d is ",sanket.result);*/
    //thread_kill(&t, SIGKILL);
    printf("i am out\n");
    sleep(5);
    while(1){
        printf("Hello in main:");
        sleep(1);
    }
    sleep(5);
    // while(1){
    // printf("i poiu\n");
    // sleep(10);
    // }
    return 0;
}
