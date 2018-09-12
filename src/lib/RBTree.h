/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: RBTree.h / RBTree.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2015/2/6 21:16:45
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2015/2/6 21:16:45
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: �������ͨ��ʵ�ַ���
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __RBTREE_H__
#define __RBTREE_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************ͷ�ļ�����***********************************************/




/***********************************************�궨��*************************************************/
#define RB_TREE_LOCK(mutex)
#define RB_TREE_UNLOCK(mutex)

/**********************************************ö�ٶ���************************************************/
typedef enum __TreeColor //�������������ɫ��ɫ���� 
{ 
    TREE_RED = 0, 
    TREE_BLACK = 1 
}w_tree_color_e; 


/*********************************************�ṹ�嶨��***********************************************/
typedef struct __w_rbtree_node_s w_rbtree_node_s;
struct __w_rbtree_node_s //��������������� 
{ 
    w_rbtree_node_s *parent; 
    w_rbtree_node_s *left; 
    w_rbtree_node_s *right; 
    w_int32_t key; 
    w_tree_color_e color; 
}; 

typedef struct __w_rbtree_s w_rbtree_s;
struct __w_rbtree_s
{
    w_rbtree_node_s *root;
    w_rbtree_node_s _nil;
    void *treeLock;//���������
};

typedef void (*rbTreeAccess)(w_rbtree_node_s *node,void *arg);


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
//w_rbtree_node_s* rbTreeParent(w_rbtree_node_s *z); //����ĳ���ĸ�ĸ 
//w_rbtree_node_s* rbTreeLeft(w_rbtree_node_s *z); //���������� 
//w_rbtree_node_s *rbTreeRight(w_rbtree_node_s *z); //���������� 

/*
* ɾ��һ��ָ���Ľڵ�
*/
w_int32_t rbTreeInsert(w_rbtree_s *tree, w_rbtree_node_s *z) ;
/*
* ����һ���½ڵ�
*/
w_int32_t rbTreeInsert(w_rbtree_s *tree, w_rbtree_node_s *z) ; 

/*
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
*/
w_rbtree_node_s* rbTreeSearch(w_rbtree_s *tree, w_rbtree_node_s *z,rbTreeAccess access,void *arg) ;
/*
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
*/
void rbTreeMidTranverse(w_rbtree_s *tree,rbTreeAccess access,void *arg);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __RBTREE_H__
