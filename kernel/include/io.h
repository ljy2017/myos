#ifndef IO
#define IO
#include "stdint.h"
//项端口port写入一个字节
static inline void outb(uint16_t port,uint8_t data){
    asm volatile("outb %b0,%w1"::"a"(data),"Nd"(port));
}
//将addr地址处的cnt个字写入端口port
static inline void outsw(uint16_t port,const void* addr,uint32_t cnt){
    asm volatile("cld;rep outsw;":"+S"(addr),"+c"(cnt):"d"(port));
}

//从端口port读入一个字节返回
static inline uint8_t inb(uint16_t port){
    int ret;
    asm volatile("inb %w1,%b0":"=a"(ret):"Nd"(port));
    return ret;
}

//从端口读入的cnt个字节写入addr中
static inline void insw(uint16_t port,const void* addr,uint32_t cnt){
    asm volatile("cld;rep insw":"+D"(addr),"+c"(cnt):"d"(port):"memory");
}
#endif