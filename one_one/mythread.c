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
#include <syscall.h>
#include <sys/wait.h>
#include "mythread.h"
#include "lock.h"

th_linked_list thread_chain;

struct spinlock test;

enum threadState{RUNNABLE,TERMINATED,RUNNING,EMBRYO,WAITING};


void traverse(){
    printf("heypoiu\n");
    node *n = thread_chain.start;
    while(n){
        if(!n->th)
            printf("lok\n");
        printf("%lu is tid \n",n->th->tid); 
        printf("%d is status\n",n->fD->status); 
        printf("kid is : %ld\n",n->th->kid);
        n=n->next;
    }
    return;
    /*
    for(int i=0;i<thread_chain.count;i++){
        printf("%ld is tid status is : %d , kid is : %ld\n",n->th->tid, n->fD->status, n->th->kid);
        n = n->next;
    }*/
}

int wrapper(void *arg){
    // funcDesc *f =(funcDesc *)arg;
    node *n ;
    // traverse();
    printf("\n");
    n = (node*)arg;
	n->fD->fPtr(n->fD->args);
	//printf("|Function excuted completely|\n");
    // f->status = 1;
    // traverse();
    // printf("Thread exited!\n");
    thread_exit(NULL);
	return 0;
}




void add_thread_to_ll(node* newNode,thDesc *t, funcDesc *f){
    newNode->th = t;
    newNode->fD = f;
    newNode->next = NULL;
    newNode->th->tid = thread_chain.count;
    (thread_chain.count)++;
    acquire(&test);
    
    newNode->next = thread_chain.start;
    thread_chain.start = newNode;
    release(&test);
    // traverse();
    // for(int i =0; i < 10000; i++);
    // printf("%c" ,'\0');
    return;
}

int thread_create(mythread_t *tt,void *attr, void *func_ptr, void *arg){
    
    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    
    if(new_stack == MAP_FAILED)
	    printf("|mmap failed!|\n");
    thDesc *t = (thDesc *)malloc(sizeof(thDesc));
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    node *newNode = (node *)malloc(sizeof(node));
    if(!t || !f || !newNode) exit(1);
    f->fPtr = func_ptr;
    f->args = arg;
    f->stack = new_stack;
    f->status = RUNNING;

    t->pid = getpid();
    t->ppid = getppid();
    t->tid = thread_chain.count;
    
    t->kid = clone(wrapper, new_stack+GUARDPSIZE+DEFAULT_STACKSIZE,CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD |CLONE_SYSVSEM|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, (void*)newNode);
    
    add_thread_to_ll(newNode,t, f);
    if(t->kid == -1){
        printf("|Clone Failed!|\n");
        //adding error flag here 
        exit(1);
    }
    
    *tt = t->kid;
     
    
    return 0;
}

int thread_join(mythread_t *t, void **retval){  
    node *temp = thread_chain.start;
    acquire(&test);
    while(temp){
        //printf("\nkid : %ld %ld\n",temp->th->kid, *t);
        if(temp->th->kid == *t){
            printf("\nkid : %ld\n",temp->th->kid);
            break;
        }
        temp = temp->next;
    }
    release(&test);
    if(temp){
        printf("\nstatus : %d\n",temp->fD->status);
        //exit(1);
        while(temp->fD->status == 0)
            ;
        //exit(1);
        return 0;
    }
    else{
        printf("not found!");
        return -1;
    }

}

void thread_exit(void *retval){
    mythread_t tid = (unsigned long)gettid();
    // int *a = (int*)retval;
    // *a = 1;
    // sleep(1);
    printf("In  exit");
    //kill(SIGINT,tid);
    node* temp = thread_chain.start;
    
    acquire(&test);
    // traverse();
    while(temp && temp->th->kid != tid){
        temp = temp->next;
    }
    release(&test);
    // exit(1);
    temp->fD->status = TERMINATED;
    syscall(SYS_exit,EXIT_SUCCESS);
    return;
}

int thread_kill(mythread_t *t, int sig){
    node *n = thread_chain.start;
    pid_t pid=0;

    acquire(&test);
    while(n){
        if(n->th->kid == *t){
            pid = n->th->pid;
            break;
        }
        n=n->next;
    }
    release(&test);
    if(sig == SIGKILL || sig == SIGTERM){
        int m= tgkill(pid,*t,sig);
        if(m==-1)
            printf("Not killed!");
    }
    else if(sig == SIGSTOP){
        int m= tgkill(pid,*t,sig);
        if(m==-1)
            printf("Not Stopped!");

    }
    else if(sig == SIGCONT){
         int m= tgkill(pid,*t,sig);
        if(m==-1)
            printf("Not Continued!");

    }

}


void thread_lock(struct spinlock* sl){
    acquire(sl);
    return;
}

void thread_unlock(struct spinlock* sl){
    release(sl);
    return;
}


struct spinlock sl;
struct c{
    int a,b,result;
};
void f(){
    //struct c *cd = (struct c*)m;
    //cd->result = cd->a + cd->b;
    /*
    while(1){
        printf("hello\n");
        sleep(1);
    }
    */
    //sleep(5);
    thread_lock(&sl);
    printf("5 secs completed!\n");
    thread_unlock(&sl);
    //printf("hello");
    //sleep(3);
    //printf("after 5\n");
    //printf("%d is :",cd->result);
    return;
}

void g(){
    // printf("5 secs completed!\n");
    //sleep(2);
    thread_lock(&sl);
    printf("h-----\n");
    thread_unlock(&sl);

    return;
}
int main(){
    initlock(&test);
    initlock(&sl);
    thread_chain.start = NULL;
    // thread_chain.end = NULL;
    thread_chain.count = 0;
    int *ret = (int *)malloc(sizeof(int));
    // void** re;
    mythread_t t,k,o,l;
    thread_create(&t,NULL, f,NULL);
    thread_create(&k,NULL, g,NULL);
    thread_create(&o,NULL, f,NULL);
    thread_create(&l,NULL, g,NULL);
    printf("jjjjj");
    //thread_join(&t,re);
    /*sleep(2);
    thread_kill(&t,SIGSTOP);
    printf("No");
    thread_kill(&t,SIGCONT);
    sleep(4);
    printf("\n%d is ",sanket.result);*/
    //thread_kill(&t, SIGKILL);
    sleep(7);

    return 0;
}
