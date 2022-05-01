#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"
#include "list.h"
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
