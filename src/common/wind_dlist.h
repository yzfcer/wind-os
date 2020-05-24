/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_dlist.h
**Author: Jason Zhou
**Last Date: 2017.12.10
**Description: 双向链表的实现方法
**              
**--------------History----------------------------------------------------------------------------
Author: 
** Version: v1.0
** Date: 2017.12.10
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 2017.12.10
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DLIST_H__
#define WIND_DLIST_H__
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

typedef struct __w_dnode_s w_dnode_s;
typedef struct __w_pridnode_s w_prinode_s;
typedef struct __w_dlist_s w_dlist_s;

//Double linked list node
struct __w_dnode_s 
{
    w_dnode_s *prev;
    w_dnode_s *next;
};

//Double linked list with priority
struct __w_pridnode_s 
{
    w_dnode_s dnode;
    w_uint32_t prio;
};


//Double Linked List header
struct __w_dlist_s 
{
    w_dnode_s *head;
    w_dnode_s *tail;
};



#define DNODE_INIT(dnode) {dnode.prev = (w_dnode_s*)W_NULL;dnode.next = (w_dnode_s*)W_NULL;}
#define PRIO_DNODE_INIT(prionode) {prionode.dnode.prev = (w_dnode_s*)W_NULL;prionode.dnode.next = (w_dnode_s*)W_NULL;prionode.prio = 0;}
#define DLIST_INIT(dlist) {dlist.head = (w_dnode_s*)W_NULL;dlist.tail = (w_dnode_s*)W_NULL;}

#define PDNODE_INIT(pdnode) {pdnode->prev = (w_dnode_s*)W_NULL;pdnode->next = (w_dnode_s*)W_NULL;}
#define PPRIO_DNODE_INIT(pprionode) {pprionode->dnode.prev = (w_dnode_s*)W_NULL;pprionode->dnode.next = (w_dnode_s*)W_NULL;pprionode->prio = 0;}
#define PDLIST_INIT(pdlist) {pdlist->head = (w_dnode_s*)W_NULL;pdlist->tail = (w_dnode_s*)W_NULL;}

#define foreach_node(dnode,list) for(dnode = dlist_head(list);dnode != (w_dnode_s*)W_NULL;dnode = dnode_next(dnode))


//get double Linked List header node
w_dnode_s *dlist_head(w_dlist_s *dlist);

//get Double Linked List tail node
w_dnode_s *dlist_tail(w_dlist_s *dlist);

//get the next node of the current node
w_dnode_s *dnode_next(w_dnode_s *dnode);

//get the previous node of the current node
w_dnode_s *dnode_prev(w_dnode_s *dnode); 


//insert a node at the header of the list
void dlist_insert_head(w_dlist_s *dlist,w_dnode_s *dnode);

//insert a node at the tail of the list
void dlist_insert_tail(w_dlist_s *dlist,w_dnode_s *dnode);

//insert a node following a certain node of the list
void dlist_insert(w_dlist_s *dlist,w_dnode_s *lpAfter,w_dnode_s *dnode);

//remove the first node of the list
w_dnode_s *dlist_remove_head(w_dlist_s *dlist);

//remove the last node of the list
w_dnode_s *dlist_remove_tail(w_dlist_s *dlist);

//remove a certain node of the list
w_dnode_s *dlist_remove(w_dlist_s *dlist,w_dnode_s *dnode);

//check if the list is empty
w_bool_t dlist_is_empty(w_dlist_s *dlist);

//Get the number of nodes in the linked list
w_int32_t dlist_get_count(w_dlist_s *dlist);

//merge two linked lists into one
w_dlist_s *dlist_combine(w_dlist_s *dlist1,w_dlist_s *dlist2);

//insert a node with priority in the linked list
void dlist_insert_prio(w_dlist_s *dlist,w_prinode_s *prinode,w_uint32_t prio);

//delete the given node with priority from the linked list
w_prinode_s *dlist_remove_prio(w_dlist_s *dlist,w_prinode_s *prinode);

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif//#ifndef WIND_DLIST_H__
