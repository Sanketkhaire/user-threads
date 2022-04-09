#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mythread.h"

static thread_ll thread_chain;

static int wrapper(void *arg){
	
    funcDesc *f =(funcDesc *)arg;
	    
	f->fPtr(f->args);
	printf("In wrapper\n");
    f->status = 1;
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

//char array[4096];

int thread_create(mythread *t,void *attr, void *func_ptr, void *argv){
    //allocating stack for thread using mmap with size : 16k + 1 Guardpage(4k)
    void *new_stack = mmap(NULL,GUARDPSIZE + DEFAULT_STACKSIZE,PROT_READ|PROT_WRITE,MAP_STACK|MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    //void *new_stack = malloc(sizeof(char)*5000);
    if(new_stack == MAP_FAILED)
	    printf("ohno");
    funcDesc *f = (funcDesc *)malloc(sizeof(funcDesc));
    f->fPtr = func_ptr;
    f->args = argv;
    f->stack = new_stack;
    f->status = 0;
    add_thread_to_ll(&thread_chain, t, f);
    //printf("Hello in threaD_create");
	//printf("\n%d\n",sizeof(new_stack));
    int id = clone(wrapper, new_stack+GUARDPSIZE+DEFAULT_STACKSIZE,CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD |CLONE_SYSVSEM|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID, (void*)f);
	printf("no %d\n", id);

    //int id;
    if(id == -1){
        printf("nooo");
        //adding error flag here 
        exit(1);
    }
    t->kid = id;
}

int thread_join(mythread *t, void **retval){
    node * temp = thread_chain.start;
    int wstatus;
    printf("\nkid : %d\n",temp->th->kid);
    
    while(temp){
        printf("\nkid : %d\n",temp->th->kid);
        if(temp->th == t){
            printf("\nkids : %d\n",temp->th->kid);
            break;
        }
        temp = temp->next;
    }
    
    if(temp){
        printf("kkk");
        while(temp->fD->status == 0)
            ;
        //waitpid(temp->th->kid,&wstatus,__WCLONE);
        *retval = (void *)&wstatus;
        return 0;
    }
    else{
        printf("not found");
        return -1;
    }

}

void thread_exit(void *retval){



}

int thread_kill(mythread *t, int sig){

}

struct c{
    int a,b,result;
};
void f(){
    //struct c *cd = (struct c*)m;
    //cd->result = cd->a + cd->b;
    printf("hello");
    sleep(5);
    printf("after 5\n");
    //printf("%d is :",cd->result);
    return;
}
int main(){
    thread_chain.start = NULL;
    thread_chain.end = NULL;
    thread_chain.count = 0;
    int *ret = (int *)malloc(sizeof(int));
    mythread t;
    struct c sanket;
    sanket.a = 4;
    sanket.b = 7;
    sanket.result = 0;
    thread_create(&t,NULL, f,NULL);
    printf("jjjjj");
    thread_join(&t, (void**)&ret);
    sleep(1);
    printf("%d is ",sanket.result);
    sleep(10);
    return 0;
}
