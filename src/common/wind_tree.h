/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_var.h / wind_var.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���ں���صı����ļ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.11.27
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TREE_H__
#define WIND_TREE_H__
#include "wind_dlist.h"
typedef struct __tree_s tree_s;
struct __tree_s
{
    tree_s *parent;
    dlist_s child_list;
    //dnode_s child_node;
    dnode_s treenode;
};

w_err_t wind_tree_init(tree_s *tree);
w_err_t wind_tree_insert_child(tree_s *parent,tree_s *child); 
w_err_t wind_tree_insert_brother(tree_s *oldbrother,tree_s *newbrother); 
w_err_t wind_tree_remove_child(tree_s *parent,tree_s *child); 
w_err_t wind_tree_remove_brother(tree_s *oldbrother,tree_s *newbrother); 
w_err_t wind_tree_search(tree_s *root,tree_s *tree);
w_err_t wind_tree_visit(tree_s *root,void (*visit)(tree_s *tree));

#endif

