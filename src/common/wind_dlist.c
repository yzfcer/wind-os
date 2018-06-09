/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_list.c
**��   ��   ��: Jason Zhou
**����޸�����: 2017.12.10
**��        ��: 
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 2017.12.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2017.12.10
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __DLIST_H__
#define __DLIST_H__
#include "wind_dlist.h"

// ������ͷ������һ���ڵ�
void dlist_insert_head(dlist_s *dlist,dnode_s *dnode)
{
    dnode->prev = NULL;
    dnode->next = dlist->head;
    if(dlist->head)
        dlist->head->prev = dnode;
    else
        dlist->tail = dnode;
    dlist->head = dnode;
}

//���� ��β������һ���ڵ�
void dlist_insert_tail(dlist_s *dlist,dnode_s *dnode)
{
    dnode->next = NULL;
    dnode->prev = dlist->tail;
    if(dlist->tail)
        dlist->tail->next = dnode;
    else
        dlist->head = dnode;
    dlist->tail = dnode;
}

// ��ָ���ڵ�����һ���ڵ�
void dlist_insert(dlist_s *dlist,dnode_s *prenode,dnode_s *dnode) 
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

// ������ͷ������һ���ڵ�
dnode_s *dlist_remove_head(dlist_s *dlist) 
{
    if(dlist->head) {
        dnode_s *dnode = dlist->head;
        if(dlist->head->next)
            dlist->head->next->prev = NULL;
        else
            dlist->tail = NULL;
        dlist->head = dlist->head->next;
        dnode->prev = dnode->next = NULL;
        return dnode;
    } else {
        return NULL;
    }
}

// ������β������һ���ڵ�
dnode_s *dlist_remove_tail(dlist_s *dlist) 
{
    if(dlist->tail) {
        dnode_s *dnode = dlist->tail;
        if(dlist->tail->prev)
            dlist->tail->prev->next = NULL;
        else
            dlist->head = NULL;
        dlist->tail = dlist->tail->prev;
        dnode->prev = dnode->next = NULL;
        return dnode;
    } else {
        return NULL;
    }
}

// ��������ɾ�������ڵ�
dnode_s *dlist_remove(dlist_s *dlist,dnode_s *dnode) 
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

// ��� �����Ƿ�Ϊ��
w_bool_t dlist_is_empty(dlist_s *dlist) 
{
    if(dlist->head || dlist->tail)
        return B_FALSE;
    else
        return B_TRUE;
}

// ��ȡ�����еĽڵ���
w_int32_t dlist_get_count(dlist_s *dlist) 
{
    w_int32_t count = 0;
    dnode_s *dnode = dlist_head(dlist);
    while(dnode) {
        ++ count;
        dnode = dnode_next(dnode);
    }
    return count;
}

//�ϲ���������
dlist_s *dlist_combine(dlist_s *dlist1,dlist_s *dlist2) 
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

// ���������һ�������ȼ��ڵ�
void dlist_insert_prio(dlist_s *dlist, prinode_s *prinode,w_uint32_t prio)
{
    prinode_s *prin = NULL;
    dnode_s *dnode;
    prinode->prio = prio;
    dnode = dlist_head(dlist);
    if(dnode == NULL)
    {
        dlist_insert_tail(dlist,&prinode->dnode);
        return;
    }
    while(dnode)
    {
        prin = DLIST_OBJ(dnode,prinode_s,dnode);
        if(prin->prio <= prinode->prio)
            dnode = dnode_next(dnode);
        else
            break;
    }
    if(dnode == NULL)
        dlist_insert_tail(dlist,&prinode->dnode);
    else 
    {
        if(prin->dnode.prev)
            dlist_insert(dlist,prin->dnode.prev,&prinode->dnode);
        else
            dlist_insert_head(dlist,&prinode->dnode);
    }
}

// ��������ɾ�����������ȼ��ڵ�
prinode_s *dlist_remove_prio(dlist_s *dlist,prinode_s *prinode)
{
    dnode_s *pdnode;
    pdnode = dlist_remove(dlist,&prinode->dnode);
    return DLIST_OBJ(pdnode,prinode_s,dnode);
}

#endif//__dlist_s_H__
