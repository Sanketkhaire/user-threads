#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include "mythread.h"
#include "lock.h"

extern th_linked_list thread_chain;
// extern enum threadState{RUNNABLE,TERMINATED,RUNNING,EMBRYO};

void initlock(struct spinlock *lk)
{
  lk->locked = 0;
  lk->tid = -1;
}

void unblockSignal()
{
    sigset_t __signalList;
    sigemptyset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
    sigaddset(&__signalList, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &__signalList, NULL);
    return;
}

void blockSignal()
{
    sigset_t __signalList;
    sigemptyset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
    sigaddset(&__signalList, SIGUSR1);
    sigprocmask(SIG_BLOCK, &__signalList, NULL);
    return;
}

static inline uint
xchg(volatile uint *addr, uint newval)
{
  uint result;

  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

void
acquire(struct spinlock *lk)
{ 

  blockSignal();
  int ktid = gettid();
  node *start = thread_chain.start;
  int var;

  while(start){
      if(start->th->kid == ktid){
          break;
      }
      start = start->next;
  }

  if(!start) var = INT_MIN;
  else var = start->th->tid;

  if(lk->locked && lk->tid == var){
    perror("acquiring gandlay\n");
    exit(1);
  }

  while(xchg(&lk->locked, 1) != 0)
    ;
  

  if(!start) printf("\nkkkk\n");
  lk->tid = var;
}



void
release(struct spinlock *lk)
{   
  int ktid = gettid();
  node *start = thread_chain.start;
  int var;

  while(start){
      if(start->th->kid == ktid){
          break;
      }
      start = start->next;
  }

  if(!start) var = INT_MIN;
  else var = start->th->tid;
  
  if(!(lk->locked && lk->tid == var)){
    perror("release gandly\n");
  }

  lk->tid = -1;
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  unblockSignal();

}