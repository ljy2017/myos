#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H
#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define	 PG_RW_R  0	// R/W 属性位值, 读/执行
#define	 PG_RW_W  2	// R/W 属性位值, 读/写/执行
#define	 PG_US_S  0	// U/S 属性位值, 系统级
#define	 PG_US_U  4	// U/S 属性位值, 用户级
#define ARENA_DESC_CNT 7
#define IS_LARGE 1
#define IS_SMALL 0
#include "stdint.h"
#include "bitmap.h"
#include "list.h"

/* 内存池标记,用于判断用哪个内存池 */
enum pool_flags {
   PF_KERNEL = 1,    // 内核内存池
   PF_USER = 2	     // 用户内存池
};


/* 用于虚拟地址管理 */

struct virtual_addr {
/* 虚拟地址用到的位图结构，用于记录哪些虚拟地址被占用了。以页为单位。*/
   struct bitmap vaddr_bitmap;
/* 管理的虚拟地址 */
   uint32_t vaddr_start;
};

typedef struct mem_block {
   struct list_elem mem_elem;
}mem_block_t;

struct arena_desc {
   uint32_t block_size;
   uint32_t block_num;
   struct list block_list;
};

struct arena {
   struct arena_desc *desc;
   uint32_t free_block_num;
   bool size_flag;//如果块大小大于等于1024，为1，否则为0
};

void *arena2block(uint32_t index, struct arena* arena);
struct arena *block2arena(void *vaddr);
struct arena_desc desc_array[ARENA_DESC_CNT];
void mem_init(void);
void* get_kernel_pages(uint32_t pg_cnt);
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt);
void malloc_init(void);
uint32_t* pte_ptr(uint32_t vaddr);
uint32_t* pde_ptr(uint32_t vaddr);
uint32_t addr_v2p(uint32_t vaddr);
void* get_a_page(enum pool_flags pf, uint32_t vaddr);
void* get_user_pages(uint32_t pg_cnt);
void *sys_malloc(uint32_t size);
void sys_free(void *vaddr);
void pfree(uint32_t addr);
void rm_pte(uint32_t vaddr);
void vaddr_free(enum pool_flags PF, void *vaddr_, uint32_t cnt);
void free_page(enum pool_flags PF, void *vaddr_, uint32_t cnt);
#endif
