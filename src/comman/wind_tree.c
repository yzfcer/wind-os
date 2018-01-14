#include "wind_tree.h"
#include "wind_debug.h"
w_err_t wind_tree_init(tree_s *tree)
{
    WIND_ASSERT_RETURN(tree != NULL,ERR_NULL_POINTER);
    tree->parent = NULL;
    DLIST_INIT(tree->child_list);
    DNODE_INIT(tree->brother_node);
    return ERR_OK;
}

w_err_t wind_tree_insert_child(tree_s *parent,tree_s *child)
{
    WIND_ASSERT_RETURN(parent != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(child != NULL,ERR_NULL_POINTER);
    child->parent = parent;
    dlist_insert_tail(&parent->child_list,&child->brother_node);
    return ERR_OK;
}

w_err_t wind_tree_insert_brother(tree_s *oldbrother,tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(newbrother != NULL,ERR_NULL_POINTER);
    return wind_tree_insert_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_remove_child(tree_s *parent,tree_s *child)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(parent != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(child != NULL,ERR_NULL_POINTER);
    dnode = dlist_remove(&parent->child_list,&child->brother_node);
    WIND_ASSERT_RETURN(dnode != NULL,ERR_INVALID_PARAM);
    child->parent = NULL;
    DNODE_INIT(child->brother_node);
    return ERR_OK;
}

w_err_t wind_tree_remove_brother(tree_s *oldbrother,tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(newbrother != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(oldbrother->parent != NULL,ERR_NULL_POINTER);

    return wind_tree_remove_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_search(tree_s *root,tree_s *tree)
{
    dnode_s *dnode;
    tree_s *subtree;
    if(root == tree)
        return ERR_OK;
    WIND_ASSERT_RETURN(root != NULL,ERR_OK);
    WIND_ASSERT_RETURN(tree != NULL,ERR_OK);
    foreach_node(dnode,&root->child_list)
    {
        subtree = DLIST_OBJ(dnode,tree_s,brother_node);
        if(subtree == tree)
            return ERR_OK;
        if(ERR_OK == wind_tree_search(subtree,tree))
            return ERR_OK;
    }
    return ERR_COMMAN;
}

w_err_t wind_tree_visit(tree_s *root,void (*visit)(tree_s *tree))
{
    dnode_s *dnode;
    tree_s *subroot;
    WIND_ASSERT_RETURN(visit != NULL,ERR_OK);
    if(root)
        visit(root);
    foreach_node(dnode,&root->child_list)
    {
        subroot = DLIST_OBJ(dnode,tree_s,brother_node);
        wind_tree_visit(subroot,visit);
    }
    return ERR_OK;
}


