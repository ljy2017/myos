#ifndef __THREAD_H
#define __THREAD_H
#include "stdint.h"
#include "list.h"

typedef void thread_func(void*);


enum task_status {
    TASK_RUNNING,
    TASK_BLOCK,
    TASK_READY,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

struct intr_stack{
    
    
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_copy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    //低特权级进入高特权级时使用
    uint32_t err_code;
    void (*eip)(void);
    int32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

struct thread_stack{

    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    void (*eip)(thread_func func,void* args);

    uint32_t unused_retaddr;
    thread_func* func;
    void* func_arg;
};

struct task_struct{
    uint32_t* self_stack;
    enum task_status status;
    uint8_t priority;
    char name[16];
    
    uint8_t ticks;//剩余需要运行的时间
    uint32_t total_ticks;//一共运行了多少时间
    struct list_elem list_tag;
    struct list_elem all_tag;

    uint32_t* pgdir;
    uint32_t stack_magic;

};
struct task_struct* thread_start(char* name,int prio,thread_func* function,void* func_arg);
void schedule();
void thread_init();
struct task_struct* running_thread();
void thread_unblock(struct task_struct* pthread);
void thread_block(enum task_status status);
#endif