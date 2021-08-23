#include "stdint.h"
#include "thread.h"
#include "syscall.h"
#include "console.h"
#include "sync.h"
#include "memory.h"
extern struct pool kernel_pool, user_pool;
uint32_t sys_get_pid(void){
    return running_thread()->pid;
}

uint32_t sys_write(char *str){
    console_put_str(str);
    return 1;
}


void syscall_init(void){
    syscall_table[GETPID] = sys_get_pid;
    syscall_table[PRINT] = sys_write;

}

