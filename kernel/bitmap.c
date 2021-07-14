#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h"

void bitmap_init(struct bitmap* btmp){
    memset(btmp->bits,0,btmp->btmp_bytes_len);
}

bool bitmap_scan_test(struct bitmap* btmp,uint32_t bit_idx){
    uint32_t byte_idx=bit_idx/8;
    uint32_t bit_odd=bit_idx%8;
    return (btmp->bits[byte_idx]&(BITMAP_MASK<<bit_odd));
}

uint32_t bitmap_scan(struct bitmap* btmp,uint32_t cnt){
    uint32_t pos=0,count=0;
   
    while(pos<((btmp->btmp_bytes_len)*8)){
        if(!bitmap_scan_test(btmp,pos)){
            count++;
            if(count==cnt){
                return pos-cnt+1;
            }
            pos++;
        }else{
            count=0;
            pos++;
        }
        
    }
    
    return -1;
}

void bitmap_set(struct bitmap* btmp,uint32_t bit_idx,int8_t value){
    ASSERT((value==1)||(value==0));
    uint32_t byte_idx=bit_idx/8;
    uint32_t bit_odd=bit_idx%8;
    if(value){
        btmp->bits[byte_idx]|=(BITMAP_MASK<<bit_odd);
    }else{
        btmp->bits[byte_idx]&=~(BITMAP_MASK<<bit_odd);
    }
}