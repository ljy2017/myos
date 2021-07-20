#include "print.h"
#include "init.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"

void print(void* str);
void k_thread_a(void*);
void k_thread_b(void*);
int main(void) {
   put_str("I am kernel\n");
   init_all();
   thread_start("k_thread_a",31,k_thread_a,"argA ");
   thread_start("k_thread_b",8,k_thread_b,"argB ");
   intr_enable();
   while(1){
      console_put_str("MAIN ");
   }

   // while(1){
   //    intr_disable();
   //    put_str("MAIN ");
   //    intr_enable();
   // }
   return 0;
}

void k_thread_a(void* str){
   char* arg=str;
   
   while(1){
      console_put_str(arg);
   }

   // while(1){
   //    intr_disable();
   //    put_str(arg);
   //    intr_enable();
   // }
}

void k_thread_b(void* str){
   char* arg=str;
   
   while(1){
      console_put_str(arg);
   }

   // while(1){
   //    intr_disable();
   //    put_str(arg);
   //    intr_enable();
   // }
}
