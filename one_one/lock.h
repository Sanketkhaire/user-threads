
#ifndef LOCK_H
#define LOCK_H

struct spinlock{
    uint locked;
    mythread_t tid;
};

void initlock(struct spinlock *lk);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);

#endif
