#ifndef __LIB_KERNEL_LIST_H
#define __LIB_KERNEL_LIST_H
#include "global.h"
#include "stdint.h"
#define offset(struct_type,member) (uint32_t)(&((struct_type*)0)->member)
#define elem2entry(struct_type,member,ptr)\
        (struct_type*)((uint32_t)ptr-offset(struct_type,member))

struct list_elem{
    struct list_elem* prev;
    struct list_elem* next;
};

struct list{
    struct list_elem head;
    struct list_elem tail;
};

struct list all_list;
struct list ready_list;

void list_init(struct list* list);
void list_insert_before(struct list_elem* item,struct list_elem* before);
void list_push(struct list* list,struct list_elem* list_elem);
void list_append(struct list* list,struct list_elem* list_elem);
void list_remove_item(struct list_elem* list_elem);
struct list_elem* list_pop(struct list* list);
bool elem_find(struct list* list,struct list_elem* list_elem);
uint32_t list_len(struct list* list);
bool list_empty(struct list* list);

#endif