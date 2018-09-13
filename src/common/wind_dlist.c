/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_list.c
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
#ifndef __DLIST_H__
#define __DLIST_H__
#include "wind_dlist.h"

// 在链表头部插入一个节点
void dlist_insert_head(w_dlist_s *dlist,w_dnode_s *dnode)
{
    dnode->prev = W_NULL;
    dnode->next = dlist->head;
    if(dlist->head)
        dlist->head->prev = dnode;
    else
        dlist->tail = dnode;
    dlist->head = dnode;
}

//在链 表尾部插入一个节点
void dlist_insert_tail(w_dlist_s *dlist,w_dnode_s *dnode)
{
    dnode->next = W_NULL;
    dnode->prev = dlist->tail;
    if(dlist->tail)
        dlist->tail->next = dnode;
    else
        dlist->head = dnode;
    dlist->tail = dnode;
}

// 在指定节点后插入一个节点
void dlist_insert(w_dlist_s *dlist,w_dnode_s *prenode,w_dnode_s *dnode) 
{
    if(prenode) {
        if(prenode->next)
            prenode->next->prev = dnode;
        else
            dlist->tail = dnode;
        dnode->prev = prenode;
        dnode->next = prenode->next;
        prenode->next = dnode;
    } else {
        dlist_insert_head(dlist, dnode);
    }
}

// 从链表头部弹出一个节点
w_dnode_s *dlist_remove_head(w_dlist_s *dlist) 
{
    if(dlist->head) {
        w_dnode_s *dnode = dlist->head;
        if(dlist->head->next)
            dlist->head->next->prev = W_NULL;
        else
            dlist->tail = W_NULL;
        dlist->head = dlist->head->next;
        dnode->prev = dnode->next = W_NULL;
        return dnode;
    } else {
        return W_NULL;
    }
}

// 从链表尾部弹出一个节点
w_dnode_s *dlist_remove_tail(w_dlist_s *dlist) 
{
    if(dlist->tail) {
        w_dnode_s *dnode = dlist->tail;
        if(dlist->tail->prev)
            dlist->tail->prev->next = W_NULL;
        else
            dlist->head = W_NULL;
        dlist->tail = dlist->tail->prev;
        dnode->prev = dnode->next = W_NULL;
        return dnode;
    } else {
        return W_NULL;
    }
}

// 从链表中删除给定节点
w_dnode_s *dlist_remove(w_dlist_s *dlist,w_dnode_s *dnode) 
{
    if(dnode->prev)
        dnode->prev->next = dnode->next;
    else
        dlist->head = dnode->next;
    if(dnode->next)
        dnode->next->prev = dnode->prev;
    else
        dlist->tail = dnode->prev;
    return dnode;
}

// 检查 链表是否为空
w_bool_t dlist_is_empty(w_dlist_s *dlist) 
{
    if(dlist->head || dlist->tail)
        return W_FALSE;
    else
        return W_TRUE;
}

// 获取链表中的节点数
w_int32_t dlist_get_count(w_dlist_s *dlist) 
{
    w_int32_t count = 0;
    w_dnode_s *dnode = dlist_head(dlist);
    while(dnode) {
        ++ count;
        dnode = dnode_next(dnode);
    }
    return count;
}

//合并两个链表
w_dlist_s *dlist_combine(w_dlist_s *dlist1,w_dlist_s *dlist2) 
{
    if(!dlist_is_empty(dlist2)) {
        if(!dlist_is_empty(dlist1)) {
            dlist1->tail->next = dlist2->head;
            dlist2->head->prev = dlist1->tail;
            dlist1->tail = dlist2->tail;
    } else {
        dlist1->head = dlist2->head;
        dlist1->tail = dlist2->tail;
    }
        dlist2->head = W_NULL;
        dlist2->tail = W_NULL;
    }
    return dlist1;
}

// 在链表插入一个带优先级节点
void dlist_insert_prio(w_dlist_s *dlist, w_prinode_s *prinode,w_uint32_t prio)
{
    w_prinode_s *prin = W_NULL;
    w_dnode_s *dnode;
    prinode->prio = prio;
    dnode = dlist_head(dlist);
    if(dnode == W_NULL)
    {
        dlist_insert_tail(dlist,&prinode->dnode);
        return;
    }
    while(dnode)
    {
        prin = DLIST_OBJ(dnode,w_prinode_s,dnode);
        if(prin->prio <= prinode->prio)
            dnode = dnode_next(dnode);
        else
            break;
    }
    if(dnode == W_NULL)
        dlist_insert_tail(dlist,&prinode->dnode);
    else 
    {
        if(prin->dnode.prev)
            dlist_insert(dlist,prin->dnode.prev,&prinode->dnode);
        else
            dlist_insert_head(dlist,&prinode->dnode);
    }
}

// 从链表中删除给定带优先级节点
w_prinode_s *dlist_remove_prio(w_dlist_s *dlist,w_prinode_s *prinode)
{
    w_dnode_s *pdnode;
    pdnode = dlist_remove(dlist,&prinode->dnode);
    return DLIST_OBJ(pdnode,w_prinode_s,dnode);
}

#endif//__dlist_s_H__
