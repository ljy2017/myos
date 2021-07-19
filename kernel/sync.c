#include "sync.h"
#include "stdint.h"
#include "interrupt.h"
#include "debug.h"
#include "thread.h"
#include "list.h"
void seamphore_init(struct seamphore* seam,uint8_t value){
    seam->value=value;
    list_init(&seam->waiter);
}

void lock_init(struct lock* lock){
    lock->count=0;
    lock->holder=NULL;
    seamphore_init(&lock->seam,1);
}

void seamphore_down(struct seamphore* seam){
    enum intr_status old_status=intr_disable();
    struct task_struct* cur_task=running_thread();
    while(seam->value==0){
        ASSERT(!elem_find(&seam->waiter,&cur_task->list_tag));
        list_append(&seam->waiter,&cur_task->list_tag);
        thread_block(TASK_BLOCK);
    }
    seam->value--;
    ASSERT(seam->value==0);
    intr_set_status(old_status);
}

void seamphore_up(struct seamphore* seam){
    enum intr_status old_status=intr_disable();
    struct task_struct* cur_task=running_thread();
    if(!list_empty(&seam->waiter)){
        struct task_struct* thread_blocked=elem2entry(struct task_struct,list_tag,list_pop(&seam->waiter));
        thread_unblock(thread_blocked);
    }
    seam->value++;
    intr_set_status(old_status);
}

