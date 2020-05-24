/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_dlist.c
** Author      : Jason Zhou
** Last Date   : 2017.12.10
** Description : 双向链表的实现方法
**              
**--------------History---------------------------------------------------------------------------------
Author: 
** Version     : v1.0
** Date        : 2017.12.10
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.10
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __DLIST_H__
#define __DLIST_H__
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((w_uint8_t*)(ptr))-((w_addr_t)&(((type*)0)->mbrnode)))

//Get linked list head node
w_dnode_s *dlist_head(w_dlist_s *dlist)
{
    return dlist->head;
}

//Get linked list tail node
w_dnode_s *dlist_tail(w_dlist_s *dlist)
{
    return dlist->tail;
}

//Get the next node of a given node
w_dnode_s *dnode_next(w_dnode_s *dnode) 
{
    return dnode->next;
}

//Get the previous node of the given node
w_dnode_s *dnode_prev(w_dnode_s *dnode) 
{
    return dnode->prev;
}

//Insert a node in the head of the list
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

//Insert a node at the end of the list
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

//Insert a node after the specified node
void dlist_insert(w_dlist_s *dlist,w_dnode_s *prenode,w_dnode_s *dnode) 
{
    if(prenode) 
    {
        if(prenode->next)
            prenode->next->prev = dnode;
        else
            dlist->tail = dnode;
        dnode->prev = prenode;
        dnode->next = prenode->next;
        prenode->next = dnode;
    }
    else 
    {
        dlist_insert_head(dlist,dnode);
    }
}

//Remove a node from the list header
w_dnode_s *dlist_remove_head(w_dlist_s *dlist) 
{
    if(dlist->head) 
    {
        w_dnode_s *dnode = dlist->head;
        if(dlist->head->next)
            dlist->head->next->prev = W_NULL;
        else
            dlist->tail = W_NULL;
        dlist->head = dlist->head->next;
        dnode->prev = dnode->next = W_NULL;
        return dnode;
    }
    else 
    {
        return W_NULL;
    }
}

//Remove a node from the end of the list
w_dnode_s *dlist_remove_tail(w_dlist_s *dlist) 
{
    if(dlist->tail) 
    {
        w_dnode_s *dnode = dlist->tail;
        if(dlist->tail->prev)
            dlist->tail->prev->next = W_NULL;
        else
            dlist->head = W_NULL;
        dlist->tail = dlist->tail->prev;
        dnode->prev = dnode->next = W_NULL;
        return dnode;
    }
    else 
    {
        return W_NULL;
    }
}

//Delete the given node from the linked list
w_dnode_s *dlist_remove(w_dlist_s *dlist,w_dnode_s *dnode) 
{
    if(dnode->prev)
        dnode->prev->next = dnode->next;
    else if(dlist->head == dnode)
        dlist->head = dnode->next;
    if(dnode->next)
        dnode->next->prev = dnode->prev;
    else if(dlist->tail == dnode)
        dlist->tail = dnode->prev;
    return dnode;
}

//Check if the list is empty
w_bool_t dlist_is_empty(w_dlist_s *dlist) 
{
    if(dlist->head || dlist->tail)
        return W_FALSE;
    else
        return W_TRUE;
}

//Get the number of nodes in the linked list
w_int32_t dlist_get_count(w_dlist_s *dlist) 
{
    w_int32_t count = 0;
    w_dnode_s *dnode = dlist_head(dlist);
    while(dnode) 
    {
        count ++;
        dnode = dnode_next(dnode);
    }
    return count;
}

//Merge two linked lists
w_dlist_s *dlist_combine(w_dlist_s *dlist1,w_dlist_s *dlist2) 
{
    if(!dlist_is_empty(dlist2)) 
    {
        if(!dlist_is_empty(dlist1)) 
        {
            dlist1->tail->next = dlist2->head;
            dlist2->head->prev = dlist1->tail;
            dlist1->tail = dlist2->tail;
        }
        else 
        {
            dlist1->head = dlist2->head;
            dlist1->tail = dlist2->tail;
        }
        dlist2->head = W_NULL;
        dlist2->tail = W_NULL;
    }
    return dlist1;
}

//Insert a node with priority in the linked list
void dlist_insert_prio(w_dlist_s *dlist,w_prinode_s *prinode,w_uint32_t prio)
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

//Delete the given node with priority from the linked list
w_prinode_s *dlist_remove_prio(w_dlist_s *dlist,w_prinode_s *prinode)
{
    w_dnode_s *dnode;
    dnode = dlist_remove(dlist,&prinode->dnode);
    return DLIST_OBJ(dnode,w_prinode_s,dnode);
}

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif//#ifndef WIND_DLIST_H__