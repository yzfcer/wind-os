/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_dlist.h
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



#define DNODE_INIT(dnode) {dnode.prev = W_NULL;dnode.next = W_NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.dnode.prev = W_NULL;prionode.dnode.next = W_NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = W_NULL;dlist.tail = W_NULL;}

#define PDNODE_INIT(pdnode) {pdnode->prev = W_NULL;pdnode->next = W_NULL;}
#define PPRIO_DNODE_INIT(pprionode) {pprionode->dnode.prev = W_NULL;pprionode->dnode.next = W_NULL;pprionode->prio = 0;}
#define PDLIST_INIT(pdlist) {pdlist->head = W_NULL;pdlist->tail = W_NULL;}

#define DLIST_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))
#define PRI_DLIST_OBJ(ptr,type,mbrnode) (type*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode.dnode)))
#define foreach_node(dnode,list) for(dnode = dlist_head(list);dnode != W_NULL;dnode = dnode_next(dnode))


//��ȡ����ͷ���ڵ�
w_dnode_s *dlist_head(w_dlist_s *dlist);

// ��ȡ����β���ڵ�
w_dnode_s *dlist_tail(w_dlist_s *dlist);

// ��ȡ�����ڵ����һ���ڵ�
w_dnode_s *dnode_next(w_dnode_s *dnode);

// ��ȡ�����ڵ����һ���ڵ�
w_dnode_s *dnode_prev(w_dnode_s *dnode); 


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
