
#ifndef LOCK_H
#define LOCK_H

struct spinlock{
    uint locked;
    mythread_t tid;
};

void blockSignal();
void unblockSignal();
void initlock(struct spinlock *lk);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);

#endif
