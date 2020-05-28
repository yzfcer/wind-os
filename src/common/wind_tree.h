/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_tree.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : Multi tree function module
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.11.27
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.11.27
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TREE_H__
#define WIND_TREE_H__
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

typedef struct __w_tree_s w_tree_s;
struct __w_tree_s
{
    w_tree_s *parent;
    w_dlist_s child_list;
    w_dnode_s treenode;
};

w_err_t wind_tree_init(w_tree_s *tree);
w_err_t wind_tree_insert_child(w_tree_s *parent,w_tree_s *child); 
w_err_t wind_tree_insert_brother(w_tree_s *oldbrother,w_tree_s *newbrother); 
w_err_t wind_tree_remove_child(w_tree_s *parent,w_tree_s *child); 
w_err_t wind_tree_remove_brother(w_tree_s *oldbrother,w_tree_s *newbrother); 
w_err_t wind_tree_search(w_tree_s *root,w_tree_s *tree);
w_err_t wind_tree_visit(w_tree_s *root,void (*visit)(w_tree_s *tree));

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_TREE_H__

