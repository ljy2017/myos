#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"

#define IDT_DESC_CNT 0x21
#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
//中断门描述符结构体
struct gate_desc{
    uint16_t func_offset_low_word;//偏移地址低16位
    uint16_t selector;//选择子
    uint8_t dcount;//固定值0
    uint8_t attrubute;//门描述符属性
    uint16_t func_offset_high_word;//偏移地址高16位
};

#define EFLAGS_IF 0x00000200//eflags寄存器中的if位为1
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0":"=g"(EFLAG_VAR))

static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);//构建一个中断描述符
static struct gate_desc idt[IDT_DESC_CNT];//中断描述符表

extern intr_handler intr_entry_table[IDT_DESC_CNT];

intr_handler idt_table[IDT_DESC_CNT];

char* intr_name[IDT_DESC_CNT];

static void general_intr_handler(uint8_t vec_nr){
    if(vec_nr==0x27||vec_nr==0x2f){
        return;
    }
    // put_str("int vector: 0x");
    // put_int(vec_nr);
    // put_char('\n');
}

static void exception_init(void){
    int i;
    for(i=0;i<IDT_DESC_CNT;i++){
        idt_table[i]=general_intr_handler;
        intr_name[i]="unknow";
    }

    intr_name[0] = "#DE Divide Error";
   intr_name[1] = "#DB Debug Exception";
   intr_name[2] = "NMI Interrupt";
   intr_name[3] = "#BP Breakpoint Exception";
   intr_name[4] = "#OF Overflow Exception";
   intr_name[5] = "#BR BOUND Range Exceeded Exception";
   intr_name[6] = "#UD Invalid Opcode Exception";
   intr_name[7] = "#NM Device Not Available Exception";
   intr_name[8] = "#DF Double Fault Exception";
   intr_name[9] = "Coprocessor Segment Overrun";
   intr_name[10] = "#TS Invalid TSS Exception";
   intr_name[11] = "#NP Segment Not Present";
   intr_name[12] = "#SS Stack Fault Exception";
   intr_name[13] = "#GP General Protection Exception";
   intr_name[14] = "#PF Page-Fault Exception";
   // intr_name[15] 第15项是intel保留项，未使用
   intr_name[16] = "#MF x87 FPU Floating-Point Error";
   intr_name[17] = "#AC Alignment Check Exception";
   intr_name[18] = "#MC Machine-Check Exception";
   intr_name[19] = "#XF SIMD Floating-Point Exception";
}

static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function){
    p_gdesc->func_offset_low_word=(uint32_t)function&0x0000FFFF;
    p_gdesc->selector=SELECTOR_K_CODE;
    p_gdesc->dcount=0;
    p_gdesc->attrubute=attr;
    p_gdesc->func_offset_high_word=((uint32_t)function&0xFFFF0000)>>16;
}

static void idt_desc_init(void){
    int i;
    for(i=0;i<IDT_DESC_CNT;i++){
        make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("  idt_desc_init done\n");
}

static void pic_init(void){
    outb(PIC_M_CTRL,0x11);//设置ICW1，边沿触发，级联8259，需要icw4
    outb(PIC_M_DATA,0x20);//ICW2:起始中断向量号为0x20
    outb(PIC_M_DATA,0x04);//ICW3:IR2接从片
    outb(PIC_M_DATA,0x01);//ICW4:8086模式，正常EOI

    //初始化从片
    outb(PIC_S_CTRL,0x11);//ICW1,边沿触发，级联8259，需要ICW4
    outb(PIC_S_DATA,0x28);//ICW2：起始中断向量号为0x28
    outb(PIC_S_DATA,0x02);//设置从片连接到主片的IR2引脚
    outb(PIC_S_DATA,0x01);//ICW4：8086模式，正常EOI

    //打开主片上的IR0，只接受时钟中断
    outb(PIC_M_DATA,0xfe);
    outb(PIC_S_DATA,0xff);

    put_str("pic_INIT done\n");
}

void idt_init(void){
    put_str("idt_init start\n");
    idt_desc_init();
    exception_init();
    pic_init();

    /*加载idt*/
    uint64_t idt_operand=(sizeof(idt)-1)|((uint64_t)((uint32_t)idt<<16));
    asm volatile("lidt %0"::"m"(idt_operand));
    put_str("idt_init done\n");
}

enum intr_status intr_enable(){
    enum intr_status old_status;
    if(intr_get_status()==INTR_ON){
        old_status=INTR_ON;
        return old_status;
    }else{
        old_status=INTR_OFF;
        asm volatile("sti");
        return old_status;
    }
}

enum intr_status intr_disable(){
    enum intr_status old_status;
    if(intr_get_status()==INTR_OFF){
        old_status=INTR_OFF;
        return old_status;
    }else{
        old_status=INTR_ON;
        asm volatile("cli");
        return old_status;
    }
}


enum intr_status intr_get_status(){
    uint32_t eflags=0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF&eflags)?INTR_ON:INTR_OFF;
}

enum intr_status intr_set_status(enum intr_status new_status){
    return new_status==INTR_ON?intr_enable():intr_disable();
}