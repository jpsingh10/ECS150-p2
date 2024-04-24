#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

struct semaphore {
    int sem_count;
    queue_t sem_queue;
};

sem_t sem_create(size_t count) {
    // Allocate memeory and error check
    sem_t new_sem = malloc(sizeof(struct semaphore));

    if (!new_sem)
        return NULL;

    // create, error check the sem queue
    new_sem->sem_queue = queue_create();
    if (!new_sem->sem_queue) {
        free(new_sem);
        return NULL;
    }
    // set the counter for the sem queue
    new_sem->sem_count = count;
    return new_sem;
}

int sem_destroy(sem_t sem) {
    if (!sem || queue_length(sem->sem_queue) > 0)
        return -1;

    queue_destroy(sem->sem_queue);
    free(sem);
    return 0;
}

int sem_down(sem_t sem) {
    if (!sem)
        return -1;

    // enqueue the current thread it to the semaphore and block it
    if (sem->sem_count == 0) {
        queue_enqueue(sem->sem_queue, uthread_current());
        uthread_block();
    }
    sem->sem_count--;
    return 0;
}

int sem_up(sem_t sem) {
    if (!sem)
        return -1;
    // deuque the ready thread in semaphore and unblock it
    if (queue_length(sem->sem_queue) > 0) {
        struct uthread_tcb *ready;
        queue_dequeue(sem->sem_queue, (void **)&ready);
        uthread_unblock(ready);
    }
    sem->sem_count++;
    return 0;
}
