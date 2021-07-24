#include "print.h"
#include "init.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"
#include "keyboard.h"
#include "ioqueue.h"

void print(void* str);
void k_thread_a(void*);
void k_thread_b(void*);
int main(void) {
   put_str("I am kernel\n");
   init_all();
   thread_start("k_thread_a",31,k_thread_a,"A_");
   thread_start("k_thread_b",8,k_thread_b,"B_");
   intr_enable();
   while(1);
   return 0;
}

void k_thread_a(void* str){
   char* arg=str;
   
   while(1){
      char ch=que_getchar(&keyboard_que);
      console_put_str(str);
      console_put_str(ch);
   }
}

void k_thread_b(void* str){
   char* arg=str;
   
   while(1){
      
      char ch=que_getchar(&keyboard_que);
      console_put_str(str);
      console_put_str(ch);
   }
}
