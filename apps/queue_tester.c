#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)                                                    \
    do {                                                                       \
        printf("ASSERT: " #assert " ... ");                                    \
        if (assert) {                                                          \
            printf("PASS\n");                                                  \
        } else {                                                               \
            printf("FAIL\n");                                                  \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

/* Create */
void test_create(void) {
    fprintf(stderr, "\n*** TEST create ***\n");

    TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
    int data = 3, *ptr;
    queue_t q;

    fprintf(stderr, "\n*** TEST queue_simple ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data);
}

/* Enqueue multiple items */
void test_queue_enqueue_multiple(void) {
    int data1 = 10, data2 = 20, data3 = 30, *ptr;
    queue_t q;

    fprintf(stderr, "\n*** TEST queue_enqueue_multiple ***\n");

    q = queue_create();
    queue_enqueue(q, &data1);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);

    TEST_ASSERT(queue_length(q) == 3);

    // Ensure the order is correct
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data1);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data2);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data3);

    queue_destroy(q);
}

void test_queue_destroy(void) {
    fprintf(stderr, "\n*** TEST queue_destroy ***\n");

    queue_t q = queue_create();
    TEST_ASSERT(queue_destroy(q) == 0);
    int data = 4;
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_destroy(q) == -1);
}

void test_queue_length(void) {
    int data = 5;
    int *ptr;
    fprintf(stderr, "\n*** TEST queue_length ***\n");

    queue_t q = queue_create();

    TEST_ASSERT(queue_length(q) == 0);
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_length(q) == 1);
    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(queue_length(q) == 0);
}

void test_queue_delete(void) {
    fprintf(stderr, "\n*** TEST queue_delete ***\n");

    queue_t q = queue_create();
    int data1 = 1, data2 = 2;
    queue_enqueue(q, &data1);
    queue_enqueue(q, &data2);

    TEST_ASSERT(queue_delete(q, &data1) == 0);
    TEST_ASSERT(queue_length(q) == 1);

    TEST_ASSERT(queue_delete(q, &data1) == -1);

    TEST_ASSERT(queue_delete(q, &data2) == 0);
    TEST_ASSERT(queue_length(q) == 0);

    queue_destroy(q);
}

static void iterator_inc(queue_t q, void *data) {
    int *a = (int *)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void) {
    fprintf(stderr, "\n*** TEST queue_iterator ***\n");

    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}

void test_enqueue_null(void) {
    fprintf(stderr, "\n*** TEST enqueue_null ***\n");

    queue_t q = queue_create();
    int result = queue_enqueue(q, NULL);
    TEST_ASSERT(result == -1);
    queue_destroy(q);
}

void test_dequeue_empty(void) {
    fprintf(stderr, "\n*** TEST dequeue_empty ***\n");

    queue_t q = queue_create();
    void *ptr = NULL;
    TEST_ASSERT(queue_dequeue(q, &ptr) == -1);
    TEST_ASSERT(ptr == NULL);
    queue_destroy(q);
}

void test_queue_empty_dequeue(void) {
    fprintf(stderr, "\n*** TEST queue_empty_dequeue ***\n");

    queue_t q = queue_create();
    int data = 10;
    int *ptr;

    queue_enqueue(q, &data);

    queue_dequeue(q, (void **)&ptr);
    TEST_ASSERT(ptr == &data);

    TEST_ASSERT(queue_dequeue(q, (void **)&ptr) == -1);

    queue_destroy(q);
}

// Test queue_delete
int main(void) {
    test_create();
    test_queue_simple();
    test_queue_enqueue_multiple();
    test_queue_delete();
    test_queue_destroy();
    test_queue_length();
    test_iterator();
    test_enqueue_null();
    test_dequeue_empty();
    test_queue_empty_dequeue();

    return 0;
}