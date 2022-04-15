#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "mythread.h"
#include "lock.h"

extern node* running_thread;

void initlock(struct spinlock *lk)
{
  lk->locked = 0;
}

void unblockSignal()
{
    sigset_t __signalList;
    sigemptyset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &__signalList, NULL);
    return;
}

void blockSignal()
{
    sigset_t __signalList;
    sigemptyset(&__signalList);
    sigaddset(&__signalList, SIGALRM);
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
  if(lk->locked && lk->tid == running_thread->th->tid){
    perror("acquiring gandlay\n");

    exit(1);
  }

  while(xchg(&lk->locked, 1) != 0)
    ;

    lk->tid = running_thread->th->tid;
}



void
release(struct spinlock *lk)
{
  if(!(lk->locked && lk->tid == running_thread->th->tid)){
    perror("release gandly\n");
  }

  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  unblockSignal();

}