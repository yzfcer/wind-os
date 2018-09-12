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
typedef struct __w_dnode_s w_dnode_s;
typedef struct __w_pridnode_s w_prinode_s;
typedef struct __w_dlist_s w_dlist_s;

//����ڵ�ṹ
struct __w_dnode_s 
{
    w_dnode_s *prev;
    w_dnode_s *next;
};

//�����ȼ�����ڵ�ṹ
struct __w_pridnode_s 
{
    w_dnode_s dnode;
    w_uint32_t prio;
};


//����ͷ���ṹ
struct __w_dlist_s 
{
    w_dnode_s *head;
    w_dnode_s *tail;
};



#define DNODE_INIT(dnode) {dnode.prev = NULL;dnode.next = NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.dnode.prev = NULL;prionode.dnode.next = NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = NULL;dlist.tail = NULL;}

#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
#define PRI_DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode.dnode)))

#define foreach_node(dnode,list) for(dnode = dlist_head(list);dnode != NULL;dnode = dnode_next(dnode))

//��ȡ����ͷ���ڵ�
static __INLINE__ 
w_dnode_s *dlist_head(w_dlist_s *dlist)
{
    return dlist->head;
}

// ��ȡ����β���ڵ�
static __INLINE__ 
w_dnode_s *dlist_tail(w_dlist_s *dlist)
{
    return dlist->tail;
}

// ��ȡ�����ڵ����һ���ڵ�
static __INLINE__ 
w_dnode_s *dnode_next(w_dnode_s *dnode) 
{
    return dnode->next;
}

// ��ȡ�����ڵ����һ���ڵ�
static __INLINE__ 
w_dnode_s *dnode_prev(w_dnode_s *dnode) 
{
    return dnode->prev;
}


// ������ͷ������һ���ڵ�
void dlist_insert_head(w_dlist_s *dlist,w_dnode_s *dnode);

//���� ��β������һ���ڵ�
void dlist_insert_tail(w_dlist_s *dlist,w_dnode_s *dnode);

// ��ָ���ڵ�����һ���ڵ�
void dlist_insert(w_dlist_s *dlist,w_dnode_s *lpAfter,w_dnode_s *dnode);

// ������ͷ������һ���ڵ�
w_dnode_s *dlist_remove_head(w_dlist_s *dlist);

// ������β������һ���ڵ�
w_dnode_s *dlist_remove_tail(w_dlist_s *dlist);

// ��������ɾ�������ڵ�
w_dnode_s *dlist_remove(w_dlist_s *dlist,w_dnode_s *dnode);

// ��� �����Ƿ�Ϊ��
w_bool_t dlist_is_empty(w_dlist_s *dlist);

// ��ȡ�����еĽڵ���
w_int32_t dlist_get_count(w_dlist_s *dlist);
// �ϲ���������
w_dlist_s *dlist_combine(w_dlist_s *dlist1,w_dlist_s *dlist2);

// ���������һ�������ȼ��ڵ�
void dlist_insert_prio(w_dlist_s *dlist,w_prinode_s *prinode,w_uint32_t prio);

// ��������ɾ�����������ȼ��ڵ�
w_prinode_s *dlist_remove_prio(w_dlist_s *dlist,w_prinode_s *prinode);

#endif//__dlist_s_H__
