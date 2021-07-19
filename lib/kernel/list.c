#include "list.h"
#include "interrupt.h"


void list_init(struct list* list){
    list->head.next=&list->tail;
    list->head.prev=NULL;
    list->tail.prev=&list->head;
    list->tail.next=NULL;
}

//在指定元素前面添加元素
void list_insert_before(struct list_elem* item,struct list_elem* before){
    enum intr_status old_status=intr_disable();
    before->prev->next=item;
    item->prev=before->prev;
    before->prev=item;
    item->next=before;
    intr_set_status(old_status);
}

//添加元素到队首
void list_push(struct list* list,struct list_elem* list_elem){
    list_insert_before(list_elem,list->head.next);
}

//添加元素到队尾
void list_append(struct list* list,struct list_elem* list_elem){
    list_insert_before(list_elem,&list->tail);
}
//删除某个特定元素
void list_remove_item(struct list_elem* list_elem){
    enum intr_status old_status=intr_disable();

    list_elem->prev->next=list_elem->next;
    list_elem->next->prev=list_elem->prev;

    intr_set_status(old_status);
}

//弹出头部第一个元素并返回
struct list_elem* list_pop(struct list* list){
    struct list_elem* elem=list->head.next;
    list_remove_item(elem);
    return elem;
}

//判断是否有某个元素
bool elem_find(struct list* list,struct list_elem* list_elem){
    struct list_elem* elem=list->head.next;
    while(elem!=&list->tail){
        if(elem==list_elem)return true;
        elem=elem->next;
    }
    return false;
}

uint32_t list_len(struct list* list){
    struct list_elem* elem=list->head.next;
    uint32_t count=0;
    while(elem!=&list->tail){
        count++;
        elem = elem->next;
    }
    return count;
}

bool list_empty(struct list* list){
    return (list->head.next==&list->tail)?true:false;
}