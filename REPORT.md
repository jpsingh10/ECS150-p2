# Introduction

This program implements a library for managing user-level threads, also known as
uthreads, which are lightweight processes sharing the same virtual memory space.
It implements the core operations for uthreads, including their creation,
joining, deletion, and yielding functionalities.

The program integrates semaphores as a synchronization tool among uthreads,
employing a queue to manage threads awaiting semaphore acquisition. It supports
essential semaphore operations, such as initialization, wait, and signal
mechanisms.

Additionally, the program also incorporates preemption through a timer-triggered
signal system, allowing for the periodic interruption of the current thread to
schedule another for execution.

The functionality is spread across several C files: `uthread.c` for uthreads
management, `queue.c` for queue data structures essential for semaphores,
`sem.c` for semaphore operations, and `preempt.c` for handling preemption.

Overall, this multithreading program provides a simple and efficient way to
create and manage user-level threads, and synchronize threads using semaphores.

# Project Structure

## queue.c

This file contains the implementation of a queue data structure that uses a
linked list. The queue uses a struct to represent the queue, and a struct to
represent the nodes of the queue. The queue supports the following operations:
create, enqueue, dequeue, delete, destroy, iteration through the queue, and
length.

The queue is created by allocating memory for the queue struct and initializing
the head and tail pointers to NULL. The queue is destroyed by freeing the memory
allocated for the queue struct and all the nodes in the queue, and also handles
the case if the user tries to destroy an empty queue. The enqueue operation adds
a node to the end of the queue. The dequeue operation removes a node from the
front of the queue and also checks if the user tries to destroy an empty queue.
The iteration operation allows the user to iterate through the queue and perform
an operation on each node. The length operation returns the number of nodes in
the queue. The delete operation removes a node containing specific data from the
queue.

## uthread.c

This file contains the implementation of the uthread library. The uthread
library provides functions to create, join, delete, and yield the CPU to another
thread. A thread must yield control to another instead of being interrupted by
the operating system scheduler.

The library categorizes threads into four distinct states: READY, RUNNING,
BLOCKED, and ZOMBIE. When a thread is in the READY state, it is prepared to
execute but is pending CPU allocation. A thread in the RUNNING state is actively
executing instructions. If a thread is in the BLOCKED state, it is on hold,
awaiting the availability of a specific resource or event. Finally, a thread
enters the ZOMBIE state upon completing its execution, at which point it awaits
the release of its resources, such as its stack.

The library uses a queue (ready_q) to manage the READY threads, and a queue
(dead_q) to manage the ZOMBIE threads. The data structure used by the library is
a struct to represent the thread control block (TCB), which holds the context of
the thread.

The idle thread is the first thread created by the `uthread_run` function, which
runs when no other thread is running, and it is responsible for scheduling the
next thread to run. New threads are created by the `uthread_create` function and
are added to the ready queue.

- The `uthread_yield` function is used to yield the CPU to another thread.
- The `uthread_block` function is used to block a thread, and the
`uthread_unblock` function unblocks the threads and adds it to the ready queue.
- The `uthread_exit` function is used to mark the thread as a zombie and adds it
to the dead queue and also yields the CPU to another thread.
- The `uthread_current` function returns the current running thread.

The implementation uses the ucontext library to implement context switching
between threads.

## sem.c

This file contains the implementation of the semaphore library. The semaphore
uses a queue to manage threads for synchronization.

The semaphore is created by allocating memory for the semaphore struct and
initializing the counter value and the queue by the `sem_create()` function. The
semaphore is destroyed by freeing the memory allocated for the semaphore struct
and the queue by the `sem_destroy()` function.

- `sem_down()` decreases the semaphore's count. If the counter hits zero, the
  calling thread is blocked and added to the sem waiting queue. It stays blocked
  until another thread calls `sem_up()`, which then unblocks the first thread in
  the queue.
- `sem_up()` increases the semaphore's count. If threads are waiting, it
  unblocks the first in the queue. If no threads are waiting, it returns -1,
  indicating no threads were unblocked.

## Testing

- For testing the queue, we created a test file `queue.c_tester` that tests all
the operations of the queue, and it passed all the tests.
- For testing `uthread.c` and `sem.c`, we created a test file and passed all the
provided tests.
- For testing `preempt.c`, we created a test file `preempt_test.c` that tests
the preemption, and it also passed all the tests.
