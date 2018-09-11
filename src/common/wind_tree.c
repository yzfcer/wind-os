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
#include "wind_tree.h"
#include "wind_debug.h"
w_err_t wind_tree_init(tree_s *tree)
{
    WIND_ASSERT_RETURN(tree != NULL,W_ERR_NULL);
    tree->parent = NULL;
    DLIST_INIT(tree->child_list);
    DNODE_INIT(tree->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_child(tree_s *parent,tree_s *child)
{
    WIND_ASSERT_RETURN(parent != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(child != NULL,W_ERR_NULL);
    child->parent = parent;
    dlist_insert_tail(&parent->child_list,&child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_brother(tree_s *oldbrother,tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(newbrother != NULL,W_ERR_NULL);
    return wind_tree_insert_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_remove_child(tree_s *parent,tree_s *child)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(parent != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(child != NULL,W_ERR_NULL);
    dnode = dlist_remove(&parent->child_list,&child->treenode);
    WIND_ASSERT_RETURN(dnode != NULL,W_ERR_INVALID);
    child->parent = NULL;
    //DNODE_INIT(child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_remove_brother(tree_s *oldbrother,tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(newbrother != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(oldbrother->parent != NULL,W_ERR_NULL);

    return wind_tree_remove_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_search(tree_s *root,tree_s *tree)
{
    dnode_s *dnode;
    tree_s *subtree;
    if(root == tree)
        return W_ERR_OK;
    WIND_ASSERT_RETURN(root != NULL,W_ERR_OK);
    WIND_ASSERT_RETURN(tree != NULL,W_ERR_OK);
    foreach_node(dnode,&root->child_list)
    {
        subtree = DLIST_OBJ(dnode,tree_s,treenode);
        if(subtree == tree)
            return W_ERR_OK;
        if(W_ERR_OK == wind_tree_search(subtree,tree))
            return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

w_err_t wind_tree_visit(tree_s *root,void (*visit)(tree_s *tree))
{
    dnode_s *dnode;
    tree_s *subroot;
    WIND_ASSERT_RETURN(visit != NULL,W_ERR_OK);
    if(root)
        visit(root);
    foreach_node(dnode,&root->child_list)
    {
        subroot = DLIST_OBJ(dnode,tree_s,treenode);
        wind_tree_visit(subroot,visit);
    }
    return W_ERR_OK;
}


