#ifndef SYSCALL_H
#define SYSCALL_H
#include "stdint.h"
#define GETPID 0
#define PRINT 1
#define MALLOC 2
void *syscall_table[32];
void syscall_init(void);

#define _syscall0(NUMBER)({ \
    int ret;\
    asm volatile("int $0x80"\
    : "=a"(ret)\
    : "a"(NUMBER)\
    : "memory"\
    );\
    ret;\
})

/* 一个参数的系统调用 */
#define _syscall1(NUMBER, ARG1) ({			       \
   int retval;					               \
   asm volatile (					       \
   "int $0x80"						       \
   : "=a" (retval)					       \
   : "a" (NUMBER), "b" (ARG1)				       \
   : "memory"						       \
   );							       \
   retval;						       \
})


#define _syscall2(NUMBER, ARG1, ARG2)({ \
    int ret;\
    asm volatile("int $0x80"\
    : "=a"(ret)\
    : "a"(NUMBER), "b"(ARG1), "c"(ARG2)\
    : "memory"\
    );\
    ret;\
})

#define _syscall3(NUMBER, ARG1, ARG2, ARG3)({ \
    int ret;\
    asm volatile("int $0x80"\
    : "=a"(ret)\
    : "a"(NUMBER), "b"(ARG1), "c"(ARG2), "d"(ARG3)\
    : "memory"\
    );\
    ret;\
})

uint32_t sys_get_pid(void);
uint32_t sys_write(char *str);
void* sys_malloc(uint32_t size);
#endif
