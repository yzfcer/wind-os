/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_tree.c
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
#include "wind_tree.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#define NODE_TO_TREE(node) NODEPTR_TO_ITEMPTR(node,w_tree_s,treenode)
w_err_t wind_tree_init(w_tree_s *tree)
{
    WIND_ASSERT_RETURN(tree != W_NULL,W_ERR_NULL_PTR);
    tree->parent = (w_tree_s *)W_NULL;
    DLIST_INIT(tree->child_list);
    DNODE_INIT(tree->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_child(w_tree_s *parent,w_tree_s *child)
{
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_NULL_PTR);
    child->parent = parent;
    dlist_insert_tail(&parent->child_list,&child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_brother(w_tree_s *oldbrother,w_tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(newbrother != W_NULL,W_ERR_NULL_PTR);
    return wind_tree_insert_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_remove_child(w_tree_s *parent,w_tree_s *child)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_NULL_PTR);
    dnode = dlist_remove(&parent->child_list,&child->treenode);
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    child->parent = (w_tree_s *)W_NULL;
    //DNODE_INIT(child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_remove_brother(w_tree_s *oldbrother,w_tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(newbrother != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(oldbrother->parent != W_NULL,W_ERR_NULL_PTR);

    return wind_tree_remove_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_search(w_tree_s *root,w_tree_s *tree)
{
    w_dnode_s *dnode;
    w_tree_s *subtree;
    if(root == tree)
        return W_ERR_OK;
    WIND_ASSERT_RETURN(root != W_NULL,W_ERR_OK);
    WIND_ASSERT_RETURN(tree != W_NULL,W_ERR_OK);
    foreach_node(dnode,&root->child_list)
    {
        subtree = NODE_TO_TREE(dnode);
        if(subtree == tree)
            return W_ERR_OK;
        if(W_ERR_OK == wind_tree_search(subtree,tree))
            return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

w_err_t wind_tree_visit(w_tree_s *root,void (*visit)(w_tree_s *tree))
{
    w_dnode_s *dnode;
    w_tree_s *subroot;
    WIND_ASSERT_RETURN(visit != W_NULL,W_ERR_OK);
    if(root)
        visit(root);
    foreach_node(dnode,&root->child_list)
    {
        subroot = NODE_TO_TREE(dnode);
        wind_tree_visit(subroot,visit);
    }
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
