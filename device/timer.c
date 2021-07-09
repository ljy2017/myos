#include "timer.h"
#include "io.h"
#include "print.h"

#define IRQ0_FREQUENCY 100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE INPUT_FREQUENCY/IRQ0_FREQUENCY
#define CONTRER0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_PORT 0x43

static void frequency_set(uint8_t counter_port,uint8_t counter_no,uint8_t rwl,uint8_t counter_mode,uint16_t counter_value){
    //往控制寄存器端口0x43写入控制字
    outb(PIT_CONTROL_PORT,(uint8_t)(counter_no<<6|rwl<<4|counter_mode<<1));
    //写入counter_value低8位
    outb(counter_port,(uint8_t)counter_value);
    //写入counter_value高8位
    outb(counter_port,(uint8_t)counter_value>>8);
}

void timer_init(void){
    put_str("timer_init start\n");
    frequency_set(CONTRER0_PORT,COUNTER0_NO,READ_WRITE_LATCH,COUNTER_MODE,COUNTER0_VALUE);
    put_str("timer_init done\n");
}