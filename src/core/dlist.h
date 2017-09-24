#ifndef __WIND_DLIST_H__
#define __WIND_DLIST_H__
#include "wind_type.h"
typedef struct __dnode_s dnode_s, *pdnode_s;
typedef struct __dlist_s dlist_s, *pdlist_s;

//链表数据结构
struct __dnode_s 
{
 pdnode_s prev;
 pdnode_s next;
};

//链表节点结构
struct __dlist_s 
{
 pdnode_s head;
 pdnode_s tail;
};

#define DNODE_INIT(dnode) {dnode.prev = NULL;dnode.next = NULL;}
#define DLIST_INIT(dlist) {dlist.head = NULL;dlist.tail = NULL;}
#define FIND(type,e) ((w_uint32_t)&(((type*)0)->e))
#define DLIST_OBJ(ptr,type,e) (void*)(((char*)(ptr))-FIND(type,e))
// 获取链表头部节点
static __INLINE__ 
pdnode_s dlist_head(pdlist_s dlist) 
{
    return dlist->head;
}

// 获取链表尾部节点
static __INLINE__ 
pdnode_s dlist_tail(pdlist_s dlist) 
{
    return dlist->tail;
}

// 获取给定节点的下一个节点
static __INLINE__ 
pdnode_s dnode_next(pdnode_s node) 
{
    return node->next;
}

// 获取给定节点的上一个节点
static __INLINE__
pdnode_s dnode_prev(pdnode_s node)
{
    return node->prev;
}

// 在链表头部插入一个节点
void dlist_insert_head(pdlist_s dlist,pdnode_s node);

//在链 表尾部插入一个节点
void dlist_insert_tail(pdlist_s dlist,pdnode_s node);

// 在指定节点后插入一个节点
void dlist_insert(pdlist_s dlist,pdnode_s lpAfter,pdnode_s node);

// 从链表头部弹出一个节点
pdnode_s dlist_remove_head(pdlist_s dlist);

// 从链表尾部弹出一个节点
pdnode_s dlist_remove_tail(pdlist_s dlist);

// 从链表中删除给定节点
pdnode_s dlist_remove(pdlist_s dlist,pdnode_s node);

// 检查 链表是否为空
w_bool_t dlist_is_empty(pdlist_s dlist);

// 获取链表中的节点数
w_int32_t dlist_get_count(pdlist_s dlist);
// 合并两个链表
pdlist_s dlist_combine(pdlist_s dlist1,pdlist_s dlist2);

#endif//__dlist_s_H__
