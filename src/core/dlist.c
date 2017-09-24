#ifndef __DLIST_H__
#define __DLIST_H__
#include "dlist.h"

// 在链表头部插入一个节点
void dlist_insert_head(pdlist_s dlist,pdnode_s node)
{
    node->prev = NULL;
    node->next = dlist->head;
    if(dlist->head)
        dlist->head->prev = node;
    else
        dlist->tail = node;
    dlist->head = node;
}

//在链 表尾部插入一个节点
void dlist_insert_tail(pdlist_s dlist,pdnode_s node)
{
    node->next = NULL;
    node->prev = dlist->tail;
    if(dlist->tail)
        dlist->tail->next = node;
    else
        dlist->head = node;
    dlist->tail = node;
}

// 在指定节点后插入一个节点
void dlist_insert(pdlist_s dlist,pdnode_s lpAfter,pdnode_s node) 
{
    if(lpAfter) {
        if(lpAfter->next)
            lpAfter->next->prev = node;
        else
            dlist->tail = node;
        node->prev = lpAfter;
        node->next = lpAfter->next;
        lpAfter->next = node;
    } else {
        dlist_insert_head(dlist, node);
    }
}

// 从链表头部弹出一个节点
pdnode_s dlist_remove_head(pdlist_s dlist) 
{
    if(dlist->head) {
        pdnode_s node = dlist->head;
        if(dlist->head->next)
            dlist->head->next->prev = NULL;
        else
            dlist->tail = NULL;
        dlist->head = dlist->head->next;
        node->prev = node->next = NULL;
        return node;
    } else {
        return NULL;
    }
}

// 从链表尾部弹出一个节点
pdnode_s dlist_remove_tail(pdlist_s dlist) 
{
    if(dlist->tail) {
        pdnode_s node = dlist->tail;
        if(dlist->tail->prev)
            dlist->tail->prev->next = NULL;
        else
            dlist->head = NULL;
        dlist->tail = dlist->tail->prev;
        node->prev = node->next = NULL;
        return node;
    } else {
        return NULL;
    }
}

// 从链表中删除给定节点
pdnode_s dlist_remove(pdlist_s dlist,pdnode_s node) 
{
    if(node->prev)
        node->prev->next = node->next;
    else
        dlist->head = node->next;
    if(node->next)
        node->next->prev = node->prev;
    else
        dlist->tail = node->prev;
    return node;
}

// 检查 链表是否为空
w_bool_t dlist_is_empty(pdlist_s dlist) 
{
    if(dlist->head || dlist->tail)
     return B_FALSE;
    else
     return B_TRUE;
}

// 获取链表中的节点数
w_int32_t dlist_get_count(pdlist_s dlist) 
{
    w_int32_t count = 0;
    pdnode_s node = dlist_head(dlist);
    while(node) {
        ++ count;
        node = dnode_next(node);
    }
    return count;
}

//合并两个链表
pdlist_s dlist_combine(pdlist_s dlist1,pdlist_s dlist2) 
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
        dlist2->head = NULL;
        dlist2->tail = NULL;
    }
    return dlist1;
}

#endif//__dlist_s_H__