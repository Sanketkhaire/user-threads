#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/mman.h>
#include "mythread.h"

static thread_ll thread_chain;

static int wrapper(void *arg){
    printf("In wrapper");
    funcDesc *f =(funcDesc *)arg;
    f->fPtr(f->args); 
    return 0;
}

void add_thread_to_ll(thread_ll *list,mythread *t, funcDesc *f){
    node * newNode = (node *)malloc(sizeof(struct node));
    newNode->th = t;
    newNode->fD = f;
    newNode->next = NULL;
    newNode->th->tid = list->count;
    (list->count)++;
    if(list->start== NULL){
        list->start = newNode;
        list->end = newNode;
        return;
    }
    else{
        list->end->next = newNode;
    }
    return;
}

int thread_create(mythread *t,const pthread_attr_t *attr, void (*func_ptr)(void *), void *argv){
    //allocating stack for thread using mmap with size : 16k + 1 Guardpage(4k)
    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_STACK|MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = argv;
    f->stack = new_stack;
    add_thread_to_ll(&thread_chain, t, f);
    printf("Hello in threaD_create");
    int id = clone((int(*) (void*))wrapper, f->stack + DEFAULT_STACKSIZE+GUARDPSIZE,CLONE_THREAD | CLONE_SIGHAND | CLONE_VM | CLONE_IO | CLONE_FILES | CLONE_FS, (void *)f);
    //int id;
    if(id == -1){
        printf("nooo");
        //adding error flag here 
        exit(1);
    }
    t->kid = id;
}

int thread_join(mythread *t, void **retval){




}

void thread_exit(void *retval){



}

int thread_kill(mythread *t, int sig){

}

struct c{
    int a,b,result;
};
void f(void *m){
    struct c *cd = (struct c*)m;
    cd->result = cd->a + cd->b;
    //printf(
    printf("%d is :",cd->result);
    return;
}
int main(){
    mythread t;
    struct c sanket;
    sanket.a = 4;
    sanket.b = 7;
    sanket.result = 0;
    thread_create(&t,NULL, &f,(void *)&sanket);
    printf("%d is ",sanket.result);
    return 0;
}
