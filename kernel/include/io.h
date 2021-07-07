#ifndef IO
#define IO
#include "stdint.h"
//项端口port写入一个字节
static inline void outb(uint16_t port,uint8_t data){
    asm volatile("mov %b0,%w1"::"a"(data),"Nd"(port));
}

#endif