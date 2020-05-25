/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: RBTree.h / RBTree.c
** Author      : Jason Zhou
** Last Date: 2015/2/6 21:16:45
** Description : 
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/2/6 21:16:45
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 红黑树的通用实现方法
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __RBTREE_H__
#define __RBTREE_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************header file***********************************************/




/***********************************************macros*************************************************/
#define RB_TREE_LOCK(mutex)
#define RB_TREE_UNLOCK(mutex)

/***********************************************enum*************************************************/
typedef enum __TreeColor //定义红黑树结点颜色颜色类型 
{ 
    TREE_RED = 0, 
    TREE_BLACK = 1 
}w_rbt_color_e; 


/***********************************************struct*************************************************/
typedef struct __w_rbt_node_s w_rbt_node_s;
typedef struct __w_rbtree_s w_rbtree_s;
struct __w_rbt_node_s //定义红黑树结点类型 
{ 
    w_rbt_node_s *parent; 
    w_rbt_node_s *left; 
    w_rbt_node_s *right; 
    w_int32_t key; 
    w_rbt_color_e color; 
}; 

struct __w_rbtree_s
{
    w_rbt_node_s *root;
    w_rbt_node_s _nil;
    void *lock;//红黑树枷锁
};

typedef void (*rbt_access_fn)(w_rbt_node_s *node,void *arg);


/********************************************global variable declare**********************************************/



/********************************************global function declare**********************************************/
//w_rbt_node_s* rbt_parent(w_rbt_node_s *rbnode); //返回某结点的父母 
//w_rbt_node_s* rbt_left(w_rbt_node_s *rbnode); //返回左子树 
//w_rbt_node_s *rbt_right(w_rbt_node_s *rbnode); //返回右子树 

/*
* 删除一个指定的节点
*/
w_int32_t wind_rbtree_insert(w_rbtree_s *tree, w_rbt_node_s *rbnode) ;
/*
* 插入一个新节点
*/
w_int32_t wind_rbtree_insert(w_rbtree_s *tree, w_rbt_node_s *rbnode); 

/*
* 查找特定的节点，需要外部加锁
*/
w_rbt_node_s* wind_rbtree_search(w_rbtree_s *tree, w_rbt_node_s *rbnode,rbt_access_fn access,void *arg) ;
/*
* 查找特定的节点，需要外部加锁
*/
void wind_rbtree_middle_tranverse(w_rbtree_s *tree,rbt_access_fn access,void *arg);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __RBTREE_H__
