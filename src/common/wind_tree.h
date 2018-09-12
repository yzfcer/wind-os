/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_var.h / wind_var.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: �ֵܺ��Ӷ�����νṹ�Ĳ�������
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
typedef struct __w_tree_s w_tree_s;
struct __w_tree_s
{
    w_tree_s *parent;
    w_dlist_s child_list;
    //w_dnode_s child_node;
    w_dnode_s treenode;
};

w_err_t wind_tree_init(w_tree_s *tree);
w_err_t wind_tree_insert_child(w_tree_s *parent,w_tree_s *child); 
w_err_t wind_tree_insert_brother(w_tree_s *oldbrother,w_tree_s *newbrother); 
w_err_t wind_tree_remove_child(w_tree_s *parent,w_tree_s *child); 
w_err_t wind_tree_remove_brother(w_tree_s *oldbrother,w_tree_s *newbrother); 
w_err_t wind_tree_search(w_tree_s *root,w_tree_s *tree);
w_err_t wind_tree_visit(w_tree_s *root,void (*visit)(w_tree_s *tree));

#endif

