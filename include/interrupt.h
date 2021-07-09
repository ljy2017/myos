#ifndef INTERRUPT
#define INTERRUPT
typedef void* intr_handler;
enum intr_status{
    INTR_OFF,//中断关闭
    INTR_ON//中断打开
};

enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);
#endif