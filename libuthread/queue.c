#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
    void *data;
    struct node *next;
};

struct queue {
    int length;
    struct node *head;
    struct node *tail;
};

queue_t queue_create(void) {
    // allocate,create, intialize, error check the queue
    queue_t queue = malloc(sizeof(struct queue));

    if (!queue)
        return NULL;

    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

int queue_destroy(queue_t queue) {
    // check if length is not null or queue not empty and free the memory
    if (!queue || queue_length(queue))
        return -1;

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data) {
    if (!queue || !data)
        return -1;

    // allocate,create, intialize, error check the new node
    struct node *newNode = malloc(sizeof(struct node));

    if (!newNode)
        return -1;

    newNode->data = data;
    newNode->next = NULL;

    // add node to head or tail of the queue
    if (!queue->head) {
        queue->head = queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }

    queue->length++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    if (!queue || !data || !queue->head)
        return -1;

    *data = queue->head->data;
    struct node *previousFront = queue->head;

    // If the queue is now empty, update the tail to NULL
    if (!queue->head)
        queue->tail = NULL;

    // move front to the next node
    queue->head = queue->head->next;

    free(previousFront);

    queue->length--;
    return 0;
}

int queue_delete(queue_t queue, void *data) {

    if (!queue || !data || !queue->length)
        return -1;

    struct node *current = queue->head;
    struct node *previous = NULL;

    // data in head node
    if (current->data == data) {
        queue->head = current->next;
        if (queue->tail == current) {
            queue->tail = NULL;
        }
        free(current);
        queue->length--;
        return 0;
    }

    // find the node to delete
    while (current != NULL && current->data != data) {
        previous = current;
        current = current->next;
    }

    // Node not found.
    if (current == NULL) {
        return -1;
    }

    if (previous) {
        previous->next = current->next;
    }

    if (queue->tail == current) {
        queue->tail = previous;
    }

    free(current);
    queue->length--;
    return 0;
}

int queue_iterate(queue_t queue, queue_func_t func) {
    if (!queue || !func)
        return -1;

    struct node *current_node = queue->head;

    // Iterate through the queue unitl end
    while (current_node) {
        struct node *nextNode = current_node->next;
        func(queue, current_node->data);
        current_node = nextNode;
    }
    return 0;
}

int queue_length(queue_t queue) {
    if (!queue)
        return -1;

    return queue->length;
}