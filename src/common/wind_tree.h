/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_var.h / wind_var.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的内核相关的变量的集合
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
typedef struct __tree_s tree_s;
struct __tree_s
{
    tree_s *parent;
    dlist_s child_list;
    //dnode_s child_node;
    dnode_s brother_node;
};

w_err_t wind_tree_init(tree_s *tree);
w_err_t wind_tree_insert_child(tree_s *parent,tree_s *child); 
w_err_t wind_tree_insert_brother(tree_s *oldbrother,tree_s *newbrother); 
w_err_t wind_tree_remove_child(tree_s *parent,tree_s *child); 
w_err_t wind_tree_remove_brother(tree_s *oldbrother,tree_s *newbrother); 
w_err_t wind_tree_search(tree_s *root,tree_s *tree);
w_err_t wind_tree_visit(tree_s *root,void (*visit)(tree_s *tree));

#endif

