#include "thread.h"
#include "stdint.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "list.h"
#include "debug.h"

#define PAGE_SIZE 4096

extern void switch_to(struct task_struct* cur,struct task_struct* next);
static void make_main_thread(void);
static void kernel_thread(thread_func* function,void* func_arg);
static void thread_create(struct task_struct* pthread,thread_func* function,void* func_arg);
static void init_thread(struct task_struct* pthread,int prio,char* name);
static void make_main_thread(void);
struct task_struct* main_thread;
static struct list_elem* thread_tag;

struct task_struct* running_thread(){
    uint32_t esp;
    asm volatile("mov %%esp,%0":"=g"(esp));
    return (struct task_struct*)(esp&0xfffff000);
}



static void kernel_thread(thread_func* function,void* func_arg){
    intr_enable();
    function(func_arg);
}



static void thread_create(struct task_struct* pthread,thread_func* function,void* func_arg){
    pthread->self_stack-=sizeof(struct intr_stack);
    pthread->self_stack-=sizeof(struct thread_stack);

    struct thread_stack* tstack=(struct thread_stack*)pthread->self_stack;
    tstack->eip=kernel_thread;
    tstack->ebp=tstack->ebx=tstack->edi=tstack->esi=0;
    tstack->func=function;
    tstack->func_arg=func_arg;
}

static void init_thread(struct task_struct* pthread,int prio,char* name){
    memset(pthread,0,sizeof(*pthread));
    pthread->priority=prio;
    strcpy(pthread->name,name);
    if(pthread==main_thread){
        pthread->status=TASK_RUNNING;
    }else{
        pthread->status=TASK_READY;
    }
    pthread->ticks=prio;
    pthread->total_ticks=0;
    pthread->pgdir=NULL;
    
    pthread->self_stack=(uint32_t*)((uint32_t)pthread+PAGE_SIZE);
    pthread->stack_magic=0x19870916;

}

void thread_init(){
    put_str("thread_init!!!\n");
    list_init(&ready_list);
    list_init(&all_list);
    make_main_thread();
    put_str("thread_init end!!!\n");
}



struct task_struct* thread_start(char* name,int prio,thread_func function,void* func_arg){
    // put_str("hello 1\n");
    struct task_struct *pthread=get_kernel_page(1);
    init_thread(pthread,prio,name);
    thread_create(pthread,function,func_arg);
    ASSERT(!elem_find(&ready_list,&pthread->list_tag));
    list_append(&ready_list,&pthread->list_tag);
    // put_str("hello 2\n");

    ASSERT(!elem_find(&all_list,&pthread->all_tag));
    list_append(&all_list,&pthread->all_tag);
    // put_str("hello 3\n");
    return pthread;
    
}

static void make_main_thread(void){
    main_thread=running_thread();
    init_thread(main_thread,31,"main");

    ASSERT(!elem_find(&all_list,&main_thread->all_tag));
    list_append(&all_list,&main_thread->all_tag);
}

void schedule(){
    ASSERT(intr_get_status()==INTR_OFF);
    struct task_struct* cur_task=running_thread();
    if(cur_task->status==TASK_RUNNING){
        ASSERT(!elem_find(&ready_list,&cur_task->list_tag));
        list_append(&ready_list,&cur_task->list_tag);
        cur_task->ticks=cur_task->priority;
        cur_task->status=TASK_READY;
        
    }else{

    }

    ASSERT(!list_empty(&ready_list));
    thread_tag=NULL;
    thread_tag=list_pop(&ready_list);
    struct task_struct* next_task=elem2entry(struct task_struct,list_tag,thread_tag);
    next_task->status=TASK_RUNNING;
    switch_to(cur_task,next_task);
}

void thread_block(enum task_status status){
    enum intr_status old_status=intr_disable();
    ASSERT(status==TASK_WAITING||status==TASK_HANGING||status==TASK_BLOCK)
    struct task_struct* cur_task=running_thread();
    cur_task->status=status;
    schedule();
    intr_set_status(old_status);
}

void thread_unblock(struct task_struct* pthread){
    enum intr_status old_status=intr_disable();
    ASSERT(pthread->status==TASK_BLOCK||pthread->status==TASK_HANGING||pthread->status==TASK_READY);
    if(pthread->status!=TASK_READY){
        ASSERT(!elem_find(&ready_list,&pthread->list_tag));
        list_push(&ready_list,&pthread->list_tag);
        pthread->status=TASK_READY;
    }
    intr_set_status(old_status);
}