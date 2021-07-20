#include "stdint.h"
void console_init();

void console_acquire();

void console_release();

void console_put_str(char* str);

void console_put_int(int num);

void console_put_char(uint8_t ch);