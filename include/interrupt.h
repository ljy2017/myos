#ifndef INTERRUPT
#define INTERRUPT
#include "stdint.h"
#define IDT_DESC_CNT 0x30
#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
typedef void* intr_handler;
enum intr_status{
    INTR_OFF,//中断关闭
    INTR_ON//中断打开
};
struct text{
    int a;
};
intr_handler idt_table[IDT_DESC_CNT];

char* intr_name[IDT_DESC_CNT];
enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);
void register_handler(uint8_t vector_no,intr_handler function);
#endif