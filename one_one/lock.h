
#ifndef LOCK_H
#define LOCK_H

typedef struct spinlock{
    uint locked;
    mythread_t tid;
}spinlock;

void initlock(struct spinlock *lk);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);

#endif
