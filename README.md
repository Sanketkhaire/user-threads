<h1 align="center">threadHive (Multithreading library) </h1> 

## Table of Contents
- [Description](#description)
- [Multithreading Models](#multithreading_models)
- [Thread APIS Provided by library](#thread_apis)
- [Usage](#usage)
- [Contributors](#cont)

# Description <a name="description"></a>
` threadHive` is an implementation of a user-level multithreading library that provides functions like thread creation, joining, blocking, destroying of threads, and signal handling in threads.<br>
It supports user-level threads in the following 3 mapping models.
<br>
These 3 models are implemented depending on how the user threads are mapped to the kernel threads.
<ol>
    <li><strong>One-One Model</strong></li>
    <li><strong>Many-One Model</strong></li>
    <li><strong>Many-Many Model</strong></li>
</ol>
The thread in each of the above models is spawned by defining a function and its arguments, which will be processed in the thread.
<br/>

# Multithreading Models  <a name="multithreading_models"></a>
Multithreading Models provide a way for establishing relationships between user-level and kernel-level threads.

1.One One Model:(1 user-thread: 1 kernel thread)
<br/>
```
One One model maps every user thread to one kernel thread.
For each user-thread creation under this model, the corresponding kernel thread is created using a clone system call.
A newly created thread is added to the linked list of threads.
Each node of the linked list encapsulates thread schema like thread_id, the function pointer, that it will execute.
```

2.Many One Model:(n user-threads: 1 kernel thread)
<br/>
```
Many One model maps many user-level threads onto one kernel thread. 
1. For each user thread created, its context is also saved using a jump buffer in the node of a linked list of threads.
2. The first two nodes of the linked list are reserved for the main thread and scheduler thread, respectively.
3. Signal SIGALARM is used as an indicator for Timer Interrupt for context switching between threads.
4. Scheduler function is designed for scheduling many user threads and handling context switch among threads.
5. During each context switch,control will be passed to the scheduler function.
6. FCFS(First come, First serve) strategy is incorporated for scheduling which selects the proper user thread for execution on a single kernel thread.
```

3.Many Many Model:(m user-threads: n kernel threads)
<br/>
```
The many-to-many model multiplexes many user-level threads to a smaller or equal number of kernel threads.
1. The number of kernel threads required is taken as input from the user.
2. Each kernel thread created using a clone system call has its own scheduler for managing user threads onto that kernel thread.
3. Two linked lists are created for managing kernel threads and user threads separately.
```

# Thread APIs provided by Library  <a name="thread_apis"></a>
1. **`thread_create(mythread_t *t, void *attr, void *func_ptr, void *args);`**

2. **`thread_join(mythread_t *t, void **retval);`**

3. **`thread_exit(void *retval);`**

4. **`thread_kill(mythread_t *T, int sig);`**

5. **`thread_lock(struct spinlock *sl);`**

6. **`thread_unlock(struct spinlock *sl);`**

# Usage <a name="usage"></a>

1. Clone this repository using the command:
```
git clone 
```
2. Run testing code using the following commands:
```
cd user-threads
bash runall.sh
```


# Contributors   <a name="cont"></a>
<ul>
<li><a href="abhishekdharmadhikari25@gmail.com">Abhishek Jagannath Dharmadhikari</a></li>
<li><a href="sanketuk@gmail.com">Sanket Ukhaji Khaire</a></li>
</ul>
