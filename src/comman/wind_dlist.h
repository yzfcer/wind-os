/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_list.h
**创   建   人: Jason Zhou
**最后修改日期: 2017.12.10
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2017.12.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2017.12.10
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __WIND_DLIST_H__
#define __WIND_DLIST_H__
#include "wind_type.h"
typedef struct __dnode_s dnode_s;
typedef struct __pri_dnode_s prinode_s;
typedef struct __dlist_s dlist_s;

//链表节点结构
struct __dnode_s 
{
    dnode_s *prev;
    dnode_s *next;
};

//带优先级链表节点结构
struct __pri_dnode_s 
{
    dnode_s node;
    w_uint32_t prio;
};


//链表头部结构
struct __dlist_s 
{
    dnode_s *head;
    dnode_s *tail;
};



#define DNODE_INIT(dnode) {dnode.prev = NULL;dnode.next = NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.node.prev = NULL;prionode.node.next = NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = NULL;dlist.tail = NULL;}
#define FIND(type,e) ((w_uint32_t)&(((type*)0)->e))
#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
#define PRI_DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode.node)))

#define foreach_node(node,list) for(node = dlist_head(list);node != NULL;node = dnode_next(node))

//获取链表头部节点
static __INLINE__ 
dnode_s *dlist_head(dlist_s *dlist)
{
    return dlist->head;
}

// 获取链表尾部节点
static __INLINE__ 
dnode_s *dlist_tail(dlist_s *dlist)
{
    return dlist->tail;
}

// 获取给定节点的下一个节点
static __INLINE__ 
dnode_s *dnode_next(dnode_s *node) 
{
    return node->next;
}

// 获取给定节点的下一个节点
static __INLINE__ 
dnode_s *dnode_prev(dnode_s *node) 
{
    return node->prev;
}


// 在链表头部插入一个节点
void dlist_insert_head(dlist_s *dlist,dnode_s *node);

//在链 表尾部插入一个节点
void dlist_insert_tail(dlist_s *dlist,dnode_s *node);

// 在指定节点后插入一个节点
void dlist_insert(dlist_s *dlist,dnode_s *lpAfter,dnode_s *node);

// 从链表头部弹出一个节点
dnode_s *dlist_remove_head(dlist_s *dlist);

// 从链表尾部弹出一个节点
dnode_s *dlist_remove_tail(dlist_s *dlist);

// 从链表中删除给定节点
dnode_s *dlist_remove(dlist_s *dlist,dnode_s *node);

// 检查 链表是否为空
w_bool_t dlist_is_empty(dlist_s *dlist);

// 获取链表中的节点数
w_int32_t dlist_get_count(dlist_s *dlist);
// 合并两个链表
dlist_s *dlist_combine(dlist_s *dlist1,dlist_s *dlist2);

// 在链表插入一个带优先级节点
void dlist_insert_prio(dlist_s *dlist,prinode_s *prinode,w_uint32_t prio);

// 从链表中删除给定带优先级节点
prinode_s *dlist_remove_prio(dlist_s *dlist,prinode_s *prinode);

#endif//__dlist_s_H__
