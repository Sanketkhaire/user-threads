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

th_linked_list thread_chain;

static int wrapper(void *arg){
    int *s = (int*)malloc(sizeof(int));
    funcDesc *f =(funcDesc *)arg;
    
	f->fPtr(f->args);
	//printf("|Function excuted completely|\n");
    f->status = 1;
    thread_exit((void*)s);
    printf("Thread exited!");
	return 0;
}

void traverse(){
    node *n = thread_chain.start;
    for(int i=0;i<thread_chain.count;i++){
        printf("%ld is tid\n",n->th->kid);
        n = n->next;
    }
}


void add_thread_to_ll(thDesc *t, funcDesc *f){
    node *newNode = (node *)malloc(sizeof(node));
    newNode->th = t;
    newNode->fD = f;
    newNode->next = NULL;
    newNode->th->tid = thread_chain.count;
    (thread_chain.count)++;
    if(thread_chain.start== NULL){
        thread_chain.start = newNode;
        thread_chain.end = newNode;
        //printf("printed %ld\n",thread_chain.start->th->kid);
    }
    else{
        thread_chain.end->next = newNode;
    }
    return;
}

int thread_create(mythread_t *tt,void *attr, void *func_ptr, void *arg){
    
    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    
    if(new_stack == MAP_FAILED)
	    printf("|mmap failed!|\n");
    thDesc *t = (thDesc *)malloc(sizeof(thDesc));
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = arg;
    f->stack = new_stack;
    f->status = 0;

    t->pid = getpid();
    t->ppid = getppid();
    add_thread_to_ll(t, f);
    int id = clone(wrapper, new_stack+GUARDPSIZE+DEFAULT_STACKSIZE,CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD |CLONE_SYSVSEM|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, (void*)f);
    if(id == -1){
        printf("|Clone Failed!|\n");
        //adding error flag here 
        exit(1);
    }
    
    t->kid = id;
    *tt = t->kid;
    
    return 0;
}

int thread_join(mythread_t *t, void **retval){  
    node *temp = thread_chain.start;
    while(temp){
        //printf("\nkid : %ld %ld\n",temp->th->kid, *t);
        if(temp->th->kid == *t){
            printf("\nkid : %ld\n",temp->th->kid);
            break;
        }
        temp = temp->next;
    }
    
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
    mythread_t tid = gettid();
    int *a = (int*)retval;
    *a = 1;
    sleep(1);
    printf("In  exit");
    //kill(SIGINT,tid);
    syscall(SYS_exit,EXIT_SUCCESS);
    printf("\nAfter exit");
    return;
}

int thread_kill(mythread_t *t, int sig){
    node *n = thread_chain.start;
    pid_t pid=0;
    while(n){
        if(n->th->kid == *t){
            pid = n->th->pid;
            break;
        }
        n=n->next;
    }
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
    printf("5 secs completed!\n");
    //printf("hello");
    //sleep(3);
    //printf("after 5\n");
    //printf("%d is :",cd->result);
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
    //thread_create(&k,NULL, f,NULL);
    printf("jjjjj");
    //thread_join(&t,re);
    /*sleep(2);
    thread_kill(&t,SIGSTOP);
    printf("No");
    thread_kill(&t,SIGCONT);
    sleep(4);
    printf("\n%d is ",sanket.result);*/
    //thread_kill(&t, SIGKILL);
    sleep(10);

    return 0;
}
