#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H
#include "stdint.h"
#include "bitmap.h"
struct virtual_addr{
    struct bitmap vaddr_bitmap;
    uint32_t vaddr_start;
};
struct pool{
    struct bitmap pool_bitmap;
    uint32_t phy_addr_start;
    uint32_t pool_size;
};
enum pool_flags{
    PF_KERNEL =1,
    PF_USER=2
};
struct pool user_pool,kernel_pool;
void mem_init(void);
uint32_t* pte_ptr(uint32_t vaddr);
uint32_t* pde_ptr(uint32_t vaddr);
void* malloc_page(enum pool_flags pool_flags, uint32_t cnt);
void* get_kernel_page(uint32_t page_cnt);



#define PG_P_1 1
#define PG_P_0 0
#define PG_RW_R 0
#define PG_RW_W 2
#define PG_US_S 0
#define PG_US_U 4
#endif