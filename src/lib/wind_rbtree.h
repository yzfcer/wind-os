/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: RBTree.h / RBTree.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/2/6 21:16:45
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/2/6 21:16:45
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 红黑树的通用实现方法
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __RBTREE_H__
#define __RBTREE_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************头文件定义***********************************************/




/***********************************************宏定义*************************************************/
#define RB_TREE_LOCK(mutex)
#define RB_TREE_UNLOCK(mutex)

/**********************************************枚举定义************************************************/
typedef enum __TreeColor //定义红黑树结点颜色颜色类型 
{ 
    TREE_RED = 0, 
    TREE_BLACK = 1 
}w_tree_color_e; 


/*********************************************结构体定义***********************************************/
typedef struct __w_rbt_node_s w_rbt_node_s;
typedef struct __w_rbt_s w_rbt_s;
struct __w_rbt_node_s //定义红黑树结点类型 
{ 
    w_rbt_node_s *parent; 
    w_rbt_node_s *left; 
    w_rbt_node_s *right; 
    w_int32_t key; 
    w_tree_color_e color; 
}; 

struct __w_rbt_s
{
    w_rbt_node_s *root;
    w_rbt_node_s _nil;
    void *treeLock;//红黑树枷锁
};

typedef void (*rbt_access_fn)(w_rbt_node_s *node,void *arg);


/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/
//w_rbt_node_s* rbt_parent(w_rbt_node_s *z); //返回某结点的父母 
//w_rbt_node_s* rbt_left(w_rbt_node_s *z); //返回左子树 
//w_rbt_node_s *rbt_right(w_rbt_node_s *z); //返回右子树 

/*
* 删除一个指定的节点
*/
w_int32_t rbt_insert(w_rbt_s *tree, w_rbt_node_s *z) ;
/*
* 插入一个新节点
*/
w_int32_t rbt_insert(w_rbt_s *tree, w_rbt_node_s *z); 

/*
* 查找特定的节点，需要外部加锁
*/
w_rbt_node_s* rbt_search(w_rbt_s *tree, w_rbt_node_s *z,rbt_access_fn access,void *arg) ;
/*
* 查找特定的节点，需要外部加锁
*/
void rbt_middle_tranverse(w_rbt_s *tree,rbt_access_fn access,void *arg);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __RBTREE_H__
