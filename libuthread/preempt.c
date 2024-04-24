#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
int enable = 0;
suseconds_t realFrequency = 1000000 / HZ;
struct itimerval timer;
struct timeval interval;
struct sigaction sa;
sigset_t block;

void handler(int signum) {
    if (signum == SIGVTALRM) {
        uthread_yield();
    }
}

void preempt_disable(void) {
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &block, NULL);
}

void preempt_enable(void) { sigprocmask(SIG_UNBLOCK, &block, NULL); }

void preempt_start(bool preempt) {
    if (preempt) {
        sa.sa_handler = handler;
        interval.tv_usec = realFrequency;
        timer.it_interval = interval;
        setitimer(ITIMER_VIRTUAL, &timer, NULL);
        sigaction(SIGVTALRM, &sa, NULL);
    }
}

void preempt_stop(void) {
    // this is how we reset our
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;

    preempt_disable();
    setitimer(SIGVTALRM, &timer, NULL);
}