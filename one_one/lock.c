#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mythread.h"
#include "lock.h"



void initlock(spinlock *lk)
{
  lk->locked = 0;
  lk->tid = -1;
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
acquire(spinlock *lk)
{
  if(lk->locked && lk->tid == gettid()){
    perror("acquiring gandlay\n");

    exit(1);
  }

  while(xchg(&lk->locked, 1) != 0)
    ;

    lk->tid = gettid();
}



void
release(spinlock *lk)
{
  if(!(lk->locked && lk->tid == gettid())){
    perror("release gandly\n");
  }

  lk->tid = -1;
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

}