#ifndef SYSCALL_USER_H
#define SYSCALL_USER_H
#include "stdint.h"
uint32_t get_pid(void);
uint32_t write(char* str);
#endif