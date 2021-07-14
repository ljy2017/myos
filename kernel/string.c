#include "string.h"
#include "global.h"
#include "debug.h"

void memset(void* dst_,uint8_t value,uint32_t size){
    ASSERT(dst_!=NULL);
    uint8_t* dst=(uint8_t*)dst_;
    while(size-->0){
        *dst=value;
        dst++;
    }
}

void memcpy(void* dst_,const void* src_,uint32_t size){
    ASSERT(dst_!=NULL&&src_!=NULL);
    uint8_t* dst=(uint8_t*)dst_;
    const uint8_t* src=(uint8_t*)src_;
    while(size--){
        *dst++=*src++;
    }
}

//a>b返回1，a<b返回-1，a==b返回0
int memcmp(const void *a_,const void* b_,uint32_t size){
    const uint8_t* a=(const void*)a_;
    const uint8_t* b=(const void*)b_;
    ASSERT(a!=NULL&&b!=NULL);
    while(size--){
        if(*a!=*b){
            return *a>*b?1:-1;
        }
        a++;
        b++;
    }
    return 0;
}

char* strcpy(char* dst_,const char* src_){
    ASSERT(dst_!=NULL&&src_!=NULL);
    char* r=dst_;
    while((*dst_++=*src_++));
    return r;
}

uint32_t strlen(const char* str){
    ASSERT(str!=NULL);
    uint32_t len=0;
    while(*str++){
        len++;
    }
    return len;
}

int8_t strcmp(const char* a,const char* b){
    ASSERT(a!=NULL&&b!=NULL);
    while(*a!=0&&*a==*b){
        a++;
        b++;
    }
    return *a<*b?-1:*a>*b;
}

char* strchr(const char* str,const uint8_t ch){
    ASSERT(str!=NULL);
    while(*str!=0){
        if(*str==ch){
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* strrchr(const char* str,const uint8_t ch){
    ASSERT(str!=NULL);
    const char* last_char=NULL;
    while(*str!=0){
        if(*str==ch){
            last_char=str;
        }
        str++;
    }
    return (char*)last_char;
}

char* strcat(char* dst_,const char* src_){
    ASSERT(dst_!=NULL&&src_!=NULL);
    char* str=dst_;
    while(str++);
    while(*str++=*src_++);
    return dst_;
}

uint32_t strchrs(const char* str,uint8_t ch){
    ASSERT(str!=NULL);
    uint8_t count=0;
    while(*str){
        if(*str++==ch)count++;
    }
    return count;
}