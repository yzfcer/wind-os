/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_list.h
**��   ��   ��: Jason Zhou
**����޸�����: 2017.12.10
**��        ��: ˫�������ʵ�ַ���
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
#ifndef __WIND_DLIST_H__
#define __WIND_DLIST_H__
#include "wind_type.h"
typedef struct __dnode_s dnode_s;
typedef struct __pri_dnode_s prinode_s;
typedef struct __dlist_s dlist_s;

//����ڵ�ṹ
struct __dnode_s 
{
    dnode_s *prev;
    dnode_s *next;
};

//�����ȼ�����ڵ�ṹ
struct __pri_dnode_s 
{
    dnode_s dnode;
    w_uint32_t prio;
};


//����ͷ���ṹ
struct __dlist_s 
{
    dnode_s *head;
    dnode_s *tail;
};



#define DNODE_INIT(dnode) {dnode.prev = NULL;dnode.next = NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.dnode.prev = NULL;prionode.dnode.next = NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = NULL;dlist.tail = NULL;}

#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
#define PRI_DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode.dnode)))

#define foreach_node(dnode,list) for(dnode = dlist_head(list);dnode != NULL;dnode = dnode_next(dnode))

//��ȡ����ͷ���ڵ�
static __INLINE__ 
dnode_s *dlist_head(dlist_s *dlist)
{
    return dlist->head;
}

// ��ȡ����β���ڵ�
static __INLINE__ 
dnode_s *dlist_tail(dlist_s *dlist)
{
    return dlist->tail;
}

// ��ȡ�����ڵ����һ���ڵ�
static __INLINE__ 
dnode_s *dnode_next(dnode_s *dnode) 
{
    return dnode->next;
}

// ��ȡ�����ڵ����һ���ڵ�
static __INLINE__ 
dnode_s *dnode_prev(dnode_s *dnode) 
{
    return dnode->prev;
}


// ������ͷ������һ���ڵ�
void dlist_insert_head(dlist_s *dlist,dnode_s *dnode);

//���� ��β������һ���ڵ�
void dlist_insert_tail(dlist_s *dlist,dnode_s *dnode);

// ��ָ���ڵ�����һ���ڵ�
void dlist_insert(dlist_s *dlist,dnode_s *lpAfter,dnode_s *dnode);

// ������ͷ������һ���ڵ�
dnode_s *dlist_remove_head(dlist_s *dlist);

// ������β������һ���ڵ�
dnode_s *dlist_remove_tail(dlist_s *dlist);

// ��������ɾ�������ڵ�
dnode_s *dlist_remove(dlist_s *dlist,dnode_s *dnode);

// ��� �����Ƿ�Ϊ��
w_bool_t dlist_is_empty(dlist_s *dlist);

// ��ȡ�����еĽڵ���
w_int32_t dlist_get_count(dlist_s *dlist);
// �ϲ���������
dlist_s *dlist_combine(dlist_s *dlist1,dlist_s *dlist2);

// ���������һ�������ȼ��ڵ�
void dlist_insert_prio(dlist_s *dlist,prinode_s *prinode,w_uint32_t prio);

// ��������ɾ�����������ȼ��ڵ�
prinode_s *dlist_remove_prio(dlist_s *dlist,prinode_s *prinode);

#endif//__dlist_s_H__
