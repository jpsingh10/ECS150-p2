#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

queue_t ready_q;
queue_t zombie_q;
typedef enum thread_state { READY, BLOCKED, RUNNING, ZOMBIE } state;

struct uthread_tcb {
    state state;
    void *stack;
    uthread_ctx_t context;
};

struct uthread_tcb *currentThread;
uthread_ctx_t *idleContext;

struct uthread_tcb *uthread_current(void) {
    return currentThread;
}

void uthread_yield(void) {
    //
    if (queue_length(ready_q) > 0) {

        struct uthread_tcb *previousThread;

        previousThread = currentThread;
        currentThread->state = READY;
        queue_enqueue(ready_q, previousThread);

        // Dequeue the next thread from the ready queue and mark it running
        // and switch to it
        queue_dequeue(ready_q, (void **)&currentThread);
        currentThread->state = RUNNING;
        uthread_ctx_switch(&(previousThread->context),
                           &(currentThread->context));
    }
}

void uthread_exit(void) {
    struct uthread_tcb *previousThread;
    previousThread = currentThread;

    // Mark the previous thread as ZOMBIE and equeue to zombie_q
    previousThread->state = ZOMBIE;
    queue_enqueue(zombie_q, previousThread);

    // Dequeue the current thread from the ready queue,  mark it running
    queue_dequeue(ready_q, (void **)&currentThread);
    currentThread->state = RUNNING;
    uthread_ctx_switch(&(previousThread->context), &(currentThread->context));
}

int uthread_create(uthread_func_t func, void *arg) {
    // allocate memeory to the tcb and error check
    struct uthread_tcb *thread = malloc(sizeof(struct uthread_tcb));
    if (!thread)
        return -1;

    // allocate the stack and Check if stack allocation failed
    thread->stack = uthread_ctx_alloc_stack();
    thread->state = READY;

    uthread_ctx_init(&(thread->context), &(thread->stack), func, arg);
    queue_enqueue(ready_q, thread);
    return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg) {
    ready_q = queue_create();
    zombie_q = queue_create();

    if (!ready_q || !zombie_q)
        return -1;

    struct uthread_tcb *idleThread;
    currentThread = malloc(sizeof(struct uthread_tcb));
    if (!currentThread) {
        queue_destroy(ready_q);
        queue_destroy(zombie_q);
        return -1;
    }
    idleContext = malloc(sizeof(uthread_ctx_t));
    if (!idleContext) {
        free(currentThread);
        queue_destroy(ready_q);
        queue_destroy(zombie_q);
        return -1;
    }

    if (!queue_length(ready_q)) {
        idleThread = malloc(sizeof(struct uthread_tcb));
        if (!idleContext) {
            free(idleContext);
            free(currentThread);
            queue_destroy(ready_q);
            queue_destroy(zombie_q);
            return -1;
        }
        idleThread->state = RUNNING;
        idleThread->stack = uthread_ctx_alloc_stack();
        uthread_ctx_init(idleContext, &(idleThread->stack), func, arg);
        currentThread = idleThread;
    }
    // create initial thread
    uthread_create(func, arg);

    while (queue_length(ready_q)) {
        uthread_yield();

        while (queue_length(zombie_q)) {
            struct uthread_tcb *toDelete;
            queue_dequeue(zombie_q, (void **)&toDelete);
            free(toDelete);
        }
    }

    uthread_ctx_destroy_stack(idleThread->stack);
    queue_destroy(zombie_q);
    queue_destroy(ready_q);

    return 0;
}

void uthread_block(void) {
    if (currentThread) {
        struct uthread_tcb *previousThread;
        previousThread = currentThread;
        previousThread->state = BLOCKED;
        queue_dequeue(ready_q, (void **)&currentThread);
        currentThread->state = RUNNING;
        uthread_ctx_switch(&(previousThread->context),
                           &(currentThread->context));
    }
}

void uthread_unblock(struct uthread_tcb *uthread) {
    if (uthread) {
        uthread->state = READY;
        queue_enqueue(ready_q, uthread);
    }
}