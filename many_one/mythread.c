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
#include "lock.h"


th_linked_list thread_chain;

node* running_thread;

spinlock lock;

enum threadState{RUNNABLE,TERMINATED,RUNNING,EMBRYO,WAITING};


static int wrapper(){
    int *s = (int*)malloc(sizeof(int));
    //printf("In Wrapper!");
    node *curr = running_thread;
    
    funcDesc *f =(funcDesc *)curr->fD;
    f->fPtr(f->args);
    acquire(&lock);
    thread_exit(NULL);
    release(&lock);
    printf("tidd : %d\n",running_thread->fD->status);
    printf("hellor\n");
    longjmp(thread_chain.start->th->myContext,1);
	return 0;
}

void traverse(){
    printf("jjj\n");
    node *n = thread_chain.start;
    while(n){
        printf("%ld is tid status : %d count : %d\n",n->th->tid,n->fD->status,thread_chain.count);
        n = n->next;
    }
}

//Ref:https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/sigpmsk.htm



void add_thread_to_ll(thDesc *t, funcDesc *f){
    node *newNode = (node *)malloc(sizeof(node));
    newNode->th = t;
    newNode->fD = f;
    newNode->next = NULL;
    newNode->th->tid = thread_chain.count;
    acquire(&lock);
    (thread_chain.count)++;
    traverse();
    printf("numberr : %d\n",thread_chain.count);
    if(thread_chain.start && thread_chain.start == thread_chain.end){
        // printf("poiu22\n");
        newNode->next = thread_chain.start;
        thread_chain.start = newNode;
        newNode->th->tid = INT_MAX;
        release(&lock);
        return;
     }
     if(thread_chain.start && thread_chain.start->next == thread_chain.end){
        // printf("poiu\n");
        newNode->next = thread_chain.end;
        thread_chain.start->next = newNode;
        release(&lock);
        return;
     }
    if(thread_chain.start== NULL){
        thread_chain.start = newNode;
        thread_chain.end = newNode;
        // printf("printed %ld\n",thread_chain.start->th->kid);
    }
    else{
        printf("last\n");
        thread_chain.end->next = newNode;
        thread_chain.end = newNode;
    }
    release(&lock);
    return;
}

void sig_handler(){
    
    printf("in handler\n");
    
    node *curr = running_thread;
    // printf("\ninnnnnnnnnn\n");

    if(setjmp(curr->th->myContext) == 0)
        longjmp(thread_chain.start->th->myContext,1);
    
};

void my_signal_handler(node *curr){
    int temp = 0;
    int ret;
    printf("in mysig\n");
    while(temp < curr->th->sigIndex){
        tgkill(curr->th->kid,curr->th->pid,curr->th->signalArr[temp]);
        // printf("in array %d\n",curr->th->sigIndex);
        temp++;
    }
    curr->th->sigIndex = 0;
    return;
}

void scheduler(){
    //setSignals();
    acquire(&lock);
    node *current = thread_chain.start->next->next, *temp = NULL;
    release(&lock);
    if((setjmp(thread_chain.start->th->myContext)) == 0){
        thread_chain.start->next->fD->status = RUNNING;
        running_thread = thread_chain.start->next;
        unblockSignal();
        ualarm(200,0);
        longjmp(thread_chain.start->next->th->myContext,3);
    }

    running_thread->fD->status = RUNNABLE;
    running_thread = thread_chain.start;
    running_thread->fD->status = RUNNING;

    acquire(&lock);
    while(current){
        temp = current;
        current = current->next;
        printf("thread no issssss:%ld\n",temp->th->tid);
        if(!current) current = thread_chain.start->next;
        // traverse();
        
        if(temp->fD->status == RUNNABLE){
            my_signal_handler(temp);
            printf("thread no :%ld\n",temp->th->tid);
            release(&lock);
            thread_chain.start->fD->status = RUNNABLE;
            temp->fD->status = RUNNING;
            running_thread = temp;
            unblockSignal();
            ualarm(200,0);
            if(setjmp(thread_chain.start->th->myContext) == 0){
                printf("hellll\n");
                longjmp(temp->th->myContext,3);
            }
            if(temp->fD->status != TERMINATED)
                temp->fD->status = RUNNABLE;
            running_thread = thread_chain.start;
            running_thread->fD->status = RUNNING;
            acquire(&lock);
            
        }

    }
    release(&lock);
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
        running_thread = thread_chain.start;
        if((setjmp(thread_chain.start->next->th->myContext)) == 0){
            longjmp(thread_chain.start->th->myContext,3);
            //scheduler();
        }
    }
}

int thread_create(mythread_t *tt,void *attr, void *func_ptr, void *arg){
    static int flag = 0;
    if(!flag){
        flag = 1;
        initlock(&lock);
    }
    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    if(new_stack == MAP_FAILED){
        perror("okkk\n");
    }

    printf("building thread\n");
    thDesc *t = (thDesc *)malloc(sizeof(thDesc));
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = arg;
    f->stack = new_stack;
    f->status = RUNNABLE;
    // printf("hey\n");
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
    acquire(&lock);
    while(temp){
        if(temp->th->tid == *t){
            // printf("\nkids : %ld\n",temp->th->tid);
            break;
        }
        temp = temp->next;
    }
    release(&lock);
    if(temp){
        // printf("\nstatus : %d\n",temp->fD->status);
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
    node *temp = running_thread;
    temp->fD->status = TERMINATED;
    return;
}


int thread_kill(mythread_t *t, int sig){
    node *temp = thread_chain.start->next;
    // printf("\nin loopsss\n");

    acquire(&lock);
    while(temp){
        // printf("\nin loop\n");
        if(temp->th->tid == *t){
            break;
        }
        temp=temp->next;
    }
    release(&lock);
    // printf("in kelll\n");

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
                temp->fD->status = TERMINATED;
            }
            else{
                temp->th->signalArr[temp->th->sigIndex++] = sig;
            }
        }
    }
}

void thread_lock(struct spinlock *t){
    acquire(t);
}

void thread_unlock(struct spinlock *t){
    release(t);
}


struct c{
    int a,b,result;
};

void f(){
    int i = 10;
    while(i--){
        printf("Hello in f: %d\n",i);
        sleep(1);
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
    initlock(&lock);
    thread_chain.start = NULL;
    thread_chain.end = NULL;
    thread_chain.count = 0;
    int *ret = (int *)malloc(sizeof(int));
    void** re;
    mythread_t t,k;
    mythread_t arr[1000];

    thread_create(&t,NULL, f,NULL);
    thread_create(&k,NULL, g,NULL);
    
    for(int i = 0; i < 1000; i++){
        thread_create(&arr[i],NULL,g,NULL);
    }
    
    printf("jjjjjppp");
    //thread_join(&t,re);
    sleep(2);

    // thread_kill(&t,SIGTERM);
    // printf("hey");
    
    // sleep(10);
    // // exit(1);
    // thread_kill(&t,SIGCONT);    
    /*sleep(2);
    thread_kill(&t,SIGSTOP);
    printf("No");
    thread_kill(&t,SIGCONT);
    sleep(4);
    printf("\n%d is ",sanket.result);*/
    //thread_kill(&t, SIGKILL);
    printf("i am out\n");
    // while(1)
    sleep(50);
    // while(1){
    //     printf("Hello in main:");
    //     sleep(1);
    // }
    // while(1)
    sleep(15);
    printf("ouuuuut\n");
    // while(1);
    // while(1){
    // printf("i poiu\n");
    // sleep(10);
    // }
    return 0;
}
