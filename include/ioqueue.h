#ifndef IOQUEUE
#define IOQUEUE
#include "stdint.h"
#include "debug.h"
#include "thread.h"
#include "sync.h"
#define bufsize 64
struct ioqueue {
    struct lock que_lock;
    struct task_struct *consumer;
    struct task_struct *producer;
    char buf[bufsize];
    uint8_t tail,head;
};

bool que_empty (struct ioqueue *que);

bool que_full (struct ioqueue* que);

uint8_t head_next (uint8_t pos);

void que_wait(struct task_struct **waiter);

void que_wake(struct task_struct **waiter);

char que_getchar(struct ioqueue *que);

void que_putchar(struct ioqueue *que,char byte);

void que_init(struct ioqueue* que);
#endif