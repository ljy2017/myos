#include "memory.h"
#include "stdint.h"
#include "print.h"
#include "debug.h"

#define PAGE_SIZE 4096

#define MEM_BITMAP_BASE 0xc009a000

#define K_HEAP_START 0xc0100000

#define PDE_IDX(addr) ((addr&0xffc00000)>>22)
#define PTE_IDX(addr) ((addr&0x003ff000)>>12)


struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint32_t all_mem){
    put_str("mem_pool_init start\n");
    uint32_t page_table_size=256*PAGE_SIZE;
    uint32_t all_used_mem=0x100000+page_table_size;
    uint32_t all_free_mem=all_mem-all_used_mem;
    uint16_t all_free_page=all_free_mem/PAGE_SIZE;

    uint16_t kernel_free_page=all_free_page/2;
    uint16_t user_free_page=all_free_page-kernel_free_page;

    kernel_pool.pool_size=kernel_free_page*PAGE_SIZE;
    user_pool.pool_size=user_free_page*PAGE_SIZE;

    kernel_pool.phy_addr_start=all_used_mem;
    user_pool.phy_addr_start=all_used_mem+kernel_pool.pool_size;

    kernel_pool.pool_bitmap.bits=(void*)MEM_BITMAP_BASE;
    kernel_pool.pool_bitmap.btmp_bytes_len=kernel_free_page/8;
    user_pool.pool_bitmap.bits=(void*)(MEM_BITMAP_BASE+kernel_free_page/8);
    user_pool.pool_bitmap.btmp_bytes_len=user_free_page/8;
    
     /******************** 输出内存池信息 **********************/
    put_str("      kernel_pool_bitmap_start:");put_int((int)kernel_pool.pool_bitmap.bits);
    put_str(" kernel_pool_phy_addr_start:");put_int(kernel_pool.phy_addr_start);
    put_str("\n");
    put_str("      user_pool_bitmap_start:");put_int((int)user_pool.pool_bitmap.bits);
    put_str(" user_pool_phy_addr_start:");put_int(user_pool.phy_addr_start);
    put_str("\n");

    //位图置零
    bitmap_init(&user_pool.pool_bitmap);
    bitmap_init(&kernel_pool.pool_bitmap);

    kernel_vaddr.vaddr_bitmap.btmp_bytes_len=kernel_free_page/8;
    kernel_vaddr.vaddr_start=K_HEAP_START;
    kernel_vaddr.vaddr_bitmap.bits=(void*)(MEM_BITMAP_BASE+user_pool.pool_bitmap.btmp_bytes_len+kernel_pool.pool_bitmap.btmp_bytes_len);
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
}

void mem_init(){
    put_str("mem_init start\n");
    uint32_t mem_bytes_total=(*(uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done\n");
}

static void* vaddr_get(enum pool_flags pf,uint32_t pg_cnt){
    uint32_t vaddr_start=0,bit_idx_start=-1,cnt=0;
    if(pf==PF_KERNEL){
        
        bit_idx_start=bitmap_scan(&kernel_vaddr.vaddr_bitmap,pg_cnt);
        if(bit_idx_start==-1)return NULL;
        while(cnt<pg_cnt){
            bitmap_set(&kernel_vaddr.vaddr_bitmap,bit_idx_start+cnt++,1);
        }
        vaddr_start=kernel_vaddr.vaddr_start+bit_idx_start*PAGE_SIZE;
    }
    else{

    }
    return (void*)vaddr_start;
}

//获得虚拟地址vaddr对应的pte指针
uint32_t* pte_ptr(uint32_t vaddr){
    uint32_t* pte=(uint32_t*)(0xffc00000+((vaddr&0xffc00000)>>10)+PTE_IDX(vaddr)*4);
    return pte;
}

//获得虚拟地址vaddr对应的pde指针
uint32_t* pde_ptr(uint32_t vaddr){
    uint32_t* pde=(uint32_t*)((0xfffff000)+PDE_IDX(vaddr)*4);
    return pde;
}

static void* palloc(struct pool* m_pool){

    int bit_idx=bitmap_scan(&(m_pool->pool_bitmap),1);
    if(bit_idx==-1)return NULL;
    bitmap_set(&(m_pool->pool_bitmap),bit_idx,1);
    uint32_t page_phyaddr=((bit_idx*PAGE_SIZE)+m_pool->phy_addr_start);
    
    return (void*)page_phyaddr;
}

static void vir_to_real(void* _vaddr,void* _page_phyaddr){

    uint32_t vaddr=(uint32_t)_vaddr;
    uint32_t page_phyaddr=(uint32_t)_page_phyaddr;

    uint32_t* pde=pde_ptr(vaddr);

    uint32_t* pte=pte_ptr(vaddr);

    if(!(*pde&0x00000001)){//pde不存在
        put_str("hello world 5\n");
        uint32_t pde_phyaddr=(uint32_t)palloc(&kernel_pool);
        *pde=(pde_phyaddr|PG_US_U|PG_RW_W|PG_P_1);
        memset((void*)((int)pte&0xffffff000),0,PAGE_SIZE);
        *pte=(page_phyaddr|PG_US_U|PG_RW_W|PG_P_1);
        
    }else{//pde存在

        *pte=(page_phyaddr|PG_US_U|PG_RW_W|PG_P_1);

    }
}

void* malloc_page(enum pool_flags pflag,uint32_t cnt){
    void* vaddr_start=vaddr_get(pflag,cnt);
    
    if(vaddr_start==NULL)return NULL;
    uint32_t vaddr=(uint32_t)vaddr_start;
    struct pool* mem_pool=(pflag==PF_KERNEL?&kernel_pool:&user_pool);
    while(cnt--){
        void* page_phyaddr=palloc(mem_pool);
        if(page_phyaddr==NULL){
            return NULL;
        }
        vir_to_real((void*)vaddr,page_phyaddr);
        vaddr+=PAGE_SIZE;
    }
    return vaddr_start;
}

void* get_kernel_page(uint32_t page_cnt){
    void* vaddr_start=malloc_page(PF_KERNEL,page_cnt);
    if(vaddr_start!=NULL){
        memset(vaddr_start,0,PAGE_SIZE*page_cnt);
    }
    return vaddr_start;
}