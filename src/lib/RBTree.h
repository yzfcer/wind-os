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
}TreeColor_E; 


/*********************************************�ṹ�嶨��***********************************************/
typedef struct __RBTreeNode RBTreeNode_S;
struct __RBTreeNode //��������������� 
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
    void *treeLock;//���������
};

typedef void (*rbTreeAccess)(RBTreeNode_S *node,void *arg);


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
//RBTreeNode_S* rbTreeParent(RBTreeNode_S *z); //����ĳ���ĸ�ĸ 
//RBTreeNode_S* rbTreeLeft(RBTreeNode_S *z); //���������� 
//RBTreeNode_S *rbTreeRight(RBTreeNode_S *z); //���������� 

/*
* ɾ��һ��ָ���Ľڵ�
*/
int rbTreeInsert(RBTree_S *tree, RBTreeNode_S *z) ;
/*
* ����һ���½ڵ�
*/
int rbTreeInsert(RBTree_S *tree, RBTreeNode_S *z) ; 

/*
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
*/
RBTreeNode_S* rbTreeSearch(RBTree_S *tree, RBTreeNode_S *z,rbTreeAccess access,void *arg) ;
/*
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
*/
void rbTreeMidTranverse(RBTree_S *tree,rbTreeAccess access,void *arg);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __RBTREE_H__
