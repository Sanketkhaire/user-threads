<h1 align="center">user-threads</h1> 
<br/>
<div align="center">
  <h2><strong>A Multithreading library</strong></h2>
</div>

## Table of Contents
- [Description](#description)
- [Multithreading Models](#multithreading_models)
- [Thread APIS Provided by library](#thread_apis)
- [Usage](#usage)
- [Contributors](#cont)

# Description <a name="description"></a>
` threadlib ` is an implementation of user-level multithreading library which provides functions like thread creation, joining, blocking, destroying of threads, signal handling in threads.<br>
It supports user-level threads in following 3 mapping models.
<br>
These 3 models are implemented depending on how the user threads are mapped to the kernel threads.
<ol>
    <li><strong>One-One Model</strong></li>
    <li><strong>Many-One Model</strong></li>
    <li><strong>Many-Many Model</strong></li>
</ol>
Thread in each of above model is spawned by defining a function and it's arguments which will be processed in the thread.
<br/>

# Multithreading Models  <a name="multithreading_models"></a>
Multithreading Models provides a way for establishing relationship between user-level and kernel-level threads.

1.One One Model:(1 user-thread: 1 kernel thread)
<br/>
```
One One model maps every user thread to one kernel thread.
For each user-thread creation under this model, corresponding kernel thread is created using clone system call.
Newly created thread is added in the linked list of threads.
Each node of linked list encapsulates thread schema like thread_id, function pointer which it will execute.
```

2.Many One Model:(n user-threads: 1 kernel thread)
<br/>
```
Many One model maps many user-level threads onto one kernel thread. 
1. For each user thread created , its context is also saved using jump buffer in node of linked list of threads.
2. First two nodes of linked list are reserved for main thread and scheduler thread respectively.
3. Signal SIGALARM is used as indicator for Timer Interrupt for context switching between threads.
4. Scheduler function is designed for scheduling many user threads and handling context switch among threads.
5. During each context-switch,control will be passed to scheduler function.
6. FCFS(First come First serve) strategy is incorportated for scheduling which selects proper user thread for execution on single kernel thread.
```

3.Many Many Model:(m user-threads: n kernel threads)
<br/>
```
The many-to-many model multiplexes many user-level threads to a smaller or equal number of kernel threads.
1. The number of kernel threads required is taken as input from user.
2. Each kernel thread created using clone system call has its own scheduler for managing user threads onto that kernel thread.
3. Two linked lists are creating for managing kernel threads and user threads seperately.
```

# Thread APIs provided by Library  <a name="thread_apis"></a>


# Usage <a name="usage"></a>

1. Clone this repository using command:
```
git clone 
```
2. Run testing code using following commands:
```
cd user-threads
bash runall.sh
```


# Contributors   <a name="cont"></a>
<ul>
<li><a href="abhishekdharmadhikari25@gmail.com">Abhishek Jagannath Dharmadhikari</a></li>
<li><a href="sanketuk@gmail.com">Sanket Ukhaji Khaire</a></li>
</ul>
