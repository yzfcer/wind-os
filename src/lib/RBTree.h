/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: RBTree.h / RBTree.c
** 创   建   人: 周江村
** 最后修改日期: 2015/2/6 21:16:45
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
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
}TreeColor_E; 


/*********************************************结构体定义***********************************************/
typedef struct __RBTreeNode RBTreeNode_S;
struct __RBTreeNode //定义红黑树结点类型 
{ 
    RBTreeNode_S *parent; 
    RBTreeNode_S *left; 
    RBTreeNode_S *right; 
    int key; 
    TreeColor_E color; 
}; 

typedef struct __RBTree_S RBTree_S;
struct __RBTree_S
{
    RBTreeNode_S *root;
    RBTreeNode_S _nil;
    void *treeLock;//红黑树枷锁
};

typedef void (*rbTreeAccess)(RBTreeNode_S *node,void *arg);


/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/
//RBTreeNode_S* rbTreeParent(RBTreeNode_S *z); //返回某结点的父母 
//RBTreeNode_S* rbTreeLeft(RBTreeNode_S *z); //返回左子树 
//RBTreeNode_S *rbTreeRight(RBTreeNode_S *z); //返回右子树 

/*
* 删除一个指定的节点
*/
int rbTreeInsert(RBTree_S *tree, RBTreeNode_S *z) ;
/*
* 插入一个新节点
*/
int rbTreeInsert(RBTree_S *tree, RBTreeNode_S *z) ; 

/*
* 查找特定的节点，需要外部加锁
*/
RBTreeNode_S* rbTreeSearch(RBTree_S *tree, RBTreeNode_S *z,rbTreeAccess access,void *arg) ;
/*
* 查找特定的节点，需要外部加锁
*/
void rbTreeMidTranverse(RBTree_S *tree,rbTreeAccess access,void *arg);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __RBTREE_H__
