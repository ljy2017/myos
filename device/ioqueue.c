#include "stdint.h"
#include "debug.h"
#include "thread.h"
#include "sync.h"
#include "interrupt.h"
#include "ioqueue.h"


void que_init(struct ioqueue* que){
    lock_init(&que->que_lock);
    que->consumer=que->producer=NULL;
    que->tail=que->head=0;
}

bool que_empty (struct ioqueue *que) {
    return que->tail == que->head;
}

uint8_t pos_next (uint8_t pos) {
    return (pos+1)%bufsize;
}

bool que_full (struct ioqueue* que) {
    return pos_next(que->head) == que->tail;
}

void que_wait(struct task_struct **waiter){
    *waiter = running_thread();
    thread_block(TASK_BLOCK);
}

void que_wake(struct task_struct **waiter){
    thread_unblock(*waiter);
    *waiter = NULL;
}

char que_getchar(struct ioqueue *que){
    while(que_empty(que)){
        lock_acquire(&que->que_lock);
        que_wait(&que->consumer);
        lock_release(&que->que_lock);
    }
    char ch=que->buf[que->tail];
    que->tail=pos_next(que->tail);
    if(que->producer!=NULL){
        que_wake(&que->producer);
    }
    return ch;
}

void que_putchar(struct ioqueue *que,char byte){
    while(que_full(que)){
        lock_acquire(&que->que_lock);
        que_wait(&que->producer);
        lock_release(&que->que_lock);
    }
    que->buf[que->head] = byte;
    que->head = pos_next(que->head);
    if(que->consumer!=NULL){
        que_wake(&que->consumer);
    }
}