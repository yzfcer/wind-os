/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_var.h / wind_var.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 兄弟孩子多叉树形结构的操作方法
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.11.27
** 描　述: 
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

