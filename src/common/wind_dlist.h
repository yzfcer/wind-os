/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dlist.h
**创   建   人: Jason Zhou
**最后修改日期: 2017.12.10
**描        述: 双向链表的实现方法
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
typedef struct __w_dnode_s w_dnode_s;
typedef struct __w_pridnode_s w_prinode_s;
typedef struct __w_dlist_s w_dlist_s;

//链表节点结构
struct __w_dnode_s 
{
    w_dnode_s *prev;
    w_dnode_s *next;
};

//带优先级链表节点结构
struct __w_pridnode_s 
{
    w_dnode_s dnode;
    w_uint32_t prio;
};


//链表头部结构
struct __w_dlist_s 
{
    w_dnode_s *head;
    w_dnode_s *tail;
};



#define DNODE_INIT(dnode) {dnode.prev = W_NULL;dnode.next = W_NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.dnode.prev = W_NULL;prionode.dnode.next = W_NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = W_NULL;dlist.tail = W_NULL;}

#define PDNODE_INIT(pdnode) {pdnode->prev = W_NULL;pdnode->next = W_NULL;}
#define PPRIO_DNODE_INIT(pprionode) {pprionode->dnode.prev = W_NULL;pprionode->dnode.next = W_NULL;pprionode->prio = 0;}
#define PDLIST_INIT(pdlist) {pdlist->head = W_NULL;pdlist->tail = W_NULL;}

#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
#define PRI_DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode.dnode)))

#define foreach_node(dnode,list) for(dnode = dlist_head(list);dnode != W_NULL;dnode = dnode_next(dnode))

//获取链表头部节点
static __INLINE__ 
w_dnode_s *dlist_head(w_dlist_s *dlist)
{
    return dlist->head;
}

// 获取链表尾部节点
static __INLINE__ 
w_dnode_s *dlist_tail(w_dlist_s *dlist)
{
    return dlist->tail;
}

// 获取给定节点的下一个节点
static __INLINE__ 
w_dnode_s *dnode_next(w_dnode_s *dnode) 
{
    return dnode->next;
}

// 获取给定节点的下一个节点
static __INLINE__ 
w_dnode_s *dnode_prev(w_dnode_s *dnode) 
{
    return dnode->prev;
}


// 在链表头部插入一个节点
void dlist_insert_head(w_dlist_s *dlist,w_dnode_s *dnode);

//在链 表尾部插入一个节点
void dlist_insert_tail(w_dlist_s *dlist,w_dnode_s *dnode);

// 在指定节点后插入一个节点
void dlist_insert(w_dlist_s *dlist,w_dnode_s *lpAfter,w_dnode_s *dnode);

// 从链表头部弹出一个节点
w_dnode_s *dlist_remove_head(w_dlist_s *dlist);

// 从链表尾部弹出一个节点
w_dnode_s *dlist_remove_tail(w_dlist_s *dlist);

// 从链表中删除给定节点
w_dnode_s *dlist_remove(w_dlist_s *dlist,w_dnode_s *dnode);

// 检查 链表是否为空
w_bool_t dlist_is_empty(w_dlist_s *dlist);

// 获取链表中的节点数
w_int32_t dlist_get_count(w_dlist_s *dlist);
// 合并两个链表
w_dlist_s *dlist_combine(w_dlist_s *dlist1,w_dlist_s *dlist2);

// 在链表插入一个带优先级节点
void dlist_insert_prio(w_dlist_s *dlist,w_prinode_s *prinode,w_uint32_t prio);

// 从链表中删除给定带优先级节点
w_prinode_s *dlist_remove_prio(w_dlist_s *dlist,w_prinode_s *prinode);

#endif//__dlist_s_H__
