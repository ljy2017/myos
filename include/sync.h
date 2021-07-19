#ifndef __THREAD_SYNC_H
#define __THREAD_SYNC_H
#include "list.h"
#include "stdint.h"
#include "thread.h"
struct seamphore{
    uint8_t value;
    struct list waiter;
};

struct lock{
    struct seamphore seam;
    struct task_struct* holder;
    uint32_t count;
};

void seamphore_init(struct seamphore* seam,uint8_t value);
void lock_init(struct lock* lock);
void seamphore_down(struct seamphore* seam);
void seamphore_up(struct seamphore* seam);
#endif