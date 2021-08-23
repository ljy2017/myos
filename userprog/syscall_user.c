#include "syscall.h"
#include "syscall_user.h"
#include "stdint.h"
uint32_t get_pid(void){
    return _syscall0(GETPID);
}
uint32_t write(char *str){
   return _syscall1(PRINT, str);
}