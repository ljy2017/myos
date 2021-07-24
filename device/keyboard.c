#include "keyboard.h"
#include "print.h"
#include "interrupt.h"
#include "io.h"
#include "global.h"
#include "ioqueue.h"

static void keyboard_handler(){
    /* 这次中断发生前的上一次中断,以下任意三个键是否有按下 */
   bool ctrl_down_last = ctrl_status;	  
   bool shift_down_last = shift_status;
   bool caps_lock_last = caps_lock_status;

   bool break_code;
   uint16_t scancode = inb(KBD_BUF_PORT);

/* 若扫描码是e0开头的,表示此键的按下将产生多个扫描码,
 * 所以马上结束此次中断处理函数,等待下一个扫描码进来*/ 
   if (scancode == 0xe0) { 
      ext_scancode = true;    // 打开e0标记
      return;
   }

/* 如果上次是以0xe0开头,将扫描码合并 */
   if (ext_scancode) {
      scancode = ((0xe000) | scancode);
      ext_scancode = false;   // 关闭e0标记
   }   

   break_code = ((scancode & 0x0080) != 0);   // 获取break_code
   
   if (break_code) {   // 若是断码break_code(按键弹起时产生的扫描码)

   /* 由于ctrl_r 和alt_r的make_code和break_code都是两字节,
   所以可用下面的方法取make_code,多字节的扫描码暂不处理 */
      uint16_t make_code = (scancode &= 0xff7f);   // 得到其make_code(按键按下时产生的扫描码)

   /* 若是任意以下三个键弹起了,将状态置为false */
      if (make_code == ctrl_l_make || make_code == ctrl_r_make) {
	 ctrl_status = false;
      } else if (make_code == shift_l_make || make_code == shift_r_make) {
	 shift_status = false;
      } else if (make_code == alt_l_make || make_code == alt_r_make) {
	 alt_status = false;
      } /* 由于caps_lock不是弹起后关闭,所以需要单独处理 */

      return;   // 直接返回结束此次中断处理程序

   } 
   /* 若为通码,只处理数组中定义的键以及alt_right和ctrl键,全是make_code */
   else if ((scancode > 0x00 && scancode < 0x3b) || \
	       (scancode == alt_r_make) || \
	       (scancode == ctrl_r_make)) {
      bool shift = false;  // 判断是否与shift组合,用来在一维数组中索引对应的字符
      if ((scancode < 0x0e) || (scancode == 0x29) || \
	 (scancode == 0x1a) || (scancode == 0x1b) || \
	 (scancode == 0x2b) || (scancode == 0x27) || \
	 (scancode == 0x28) || (scancode == 0x33) || \
	 (scancode == 0x34) || (scancode == 0x35)) {  
	    /****** 代表两个字母的键 ********
		     0x0e 数字'0'~'9',字符'-',字符'='
		     0x29 字符'`'
		     0x1a 字符'['
		     0x1b 字符']'
		     0x2b 字符'\\'
		     0x27 字符';'
		     0x28 字符'\''
		     0x33 字符','
		     0x34 字符'.'
		     0x35 字符'/' 
	    *******************************/
	 if (shift_down_last) {  // 如果同时按下了shift键
	    shift = true;
	 }
      } else {	  // 默认为字母键
	 if (shift_down_last && caps_lock_last) {  // 如果shift和capslock同时按下
	    shift = false;
	 } else if (shift_down_last || caps_lock_last) { // 如果shift和capslock任意被按下
	    shift = true;
	 } else {
	    shift = false;
	 }
      }

      uint8_t index = (scancode &= 0x00ff);  // 将扫描码的高字节置0,主要是针对高字节是e0的扫描码.
      char cur_char = keymap[index][shift];  // 在数组中找到对应的字符
   
      /* 只处理ascii码不为0的键 */
      if (cur_char) {
	      put_char(cur_char);
         if(!que_full(&keyboard_que))
            que_putchar(&keyboard_que,cur_char);
	      return;
      }

      /* 记录本次是否按下了下面几类控制键之一,供下次键入时判断组合键 */
      if (scancode == ctrl_l_make || scancode == ctrl_r_make) {
	 ctrl_status = true;
      } else if (scancode == shift_l_make || scancode == shift_r_make) {
	 shift_status = true;
      } else if (scancode == alt_l_make || scancode == alt_r_make) {
	 alt_status = true;
      } else if (scancode == caps_lock_make) {
      /* 不管之前是否有按下caps_lock键,当再次按下时则状态取反,
       * 即:已经开启时,再按下同样的键是关闭。关闭时按下表示开启。*/
	 caps_lock_status = !caps_lock_status;
      }
   } else {
      put_str("unknown key\n");
   }
    return;
}
void keyboard_init(){
    put_str("keyboard init start\n");
    que_init(&keyboard_que);
    register_handler(0x21,keyboard_handler);
    put_str("keyboard init end!\n");
}

