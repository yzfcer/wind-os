/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml.c
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2020/03/24
  *Description: 
  *Others:      An external mutex is required when used in a multithreaded environment,
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "wind_xml.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
static w_bool_t is_xml_name_valid(char *name)
{
    w_int32_t i;
    WIND_ASSERT_RETURN(name != W_NULL,W_FALSE);

    for(i = 0;name[i] != 0;i ++)
    {
        if(IS_ALPHABET(name[i]))
            continue;
        else if(IS_DECIMAL(name[i]))
            continue;
        else if(name[i] == '_')
            continue;
        else
            return W_FALSE;
    }
    return W_TRUE;
}

static w_err_t xattr_destroy(w_xmlattr_s *attr)
{
    WIND_CHECK_RETURN(attr != W_NULL,W_ERR_PTR_NULL);
    if(attr->attr_name)
        wind_free(attr->attr_name);
    if(attr->attr_value)
        wind_free(attr->attr_value);
    wind_free(attr);
    return W_ERR_OK;
}


w_err_t wind_xml_parse(w_xmlnode_s *xnode,char *xmlstr,w_int32_t len)
{
    return W_ERR_FAIL;
}

w_err_t wind_xml_print(w_xmlnode_s *xnode)
{
    w_int32_t i;
    w_err_t err;
    w_dnode_s *dnode;
    w_xmlattr_s *attr;
    w_tree_s *tree;
    w_xmlnode_s *tmp_xndoe;
    char prefix[MAX_XNODE_LEVEL+1];
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->level < MAX_XNODE_LEVEL,W_ERR_FAIL);
    err = W_ERR_OK;
    wind_memset(prefix,0,sizeof(prefix));
    for(i = 0;i < xnode->level;i ++)
        prefix[i] = '\t';
    wind_printf(prefix);
    wind_printf("<%s ",xnode->name);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        wind_printf("\"%s\"=\"%s\" ",attr->attr_name,attr->attr_value);
    }
    wind_printf(">");
    if(xnode->value != NULL)
        wind_printf("%s",xnode->value);
    else if(dlist_head(&xnode->tree.child_list))
    {
        wind_printf("\r\n");
        foreach_node(dnode,xnode->tree.child_list)
        {
            tree = NODE_TO_TREE(dnode);
            tmp_xndoe = TREE_TO_XNODE(tree);
            err = wind_xml_print(tmp_xndoe);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"print child xnode failed");
        }
        wind_printf("\r\n");
    }
    wind_printf("</%s>\r\n",xnode->name);
    return W_ERR_OK;
}


w_xmlnode_s *wind_xmlnode_create(char *name)
{
    w_err_t err;
    w_bool_t isvalid;
    w_xmlnode_s *xnode = W_NULL;
    do
    {
        err = W_ERR_OK;
        isvalid = is_xml_name_valid(name);
        WIND_ASSERT_BREAK(isvalid == W_TRUE,W_ERR_INVALID,"node name is invalid");
        xnode = wind_malloc(sizeof(w_xmlnode_s));
        wind_memset(xnode,0,sizeof(w_xmlnode_s));
        xnode->name = wind_salloc(name,0);
        WIND_ASSERT_BREAK(xnode->name != W_NULL,W_ERR_MEM,"alloc xnode name failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        wind_xmlnode_destroy(xnode);
        xnode = W_NULL;
    }
    return xnode;
}

w_err_t wind_xmlnode_destroy(w_xmlnode_s *xnode)
{
    w_err_t err;
    w_xmlattr_s *attr;
    w_dnode_s *dnode;
    w_xmlnode_s *child;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    if(xnode->name != W_NULL)
        wind_free(xnode->name);
    if(xnode->value != W_NULL)
        wind_free(xnode->value);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        xattr_destroy(attr);
    }
    err = W_ERR_OK;
    for(;;)
    {
        child = wind_xmlnode_get_child(xnode);
        WIND_CHECK_BREAK(child != W_NULL,W_ERR_OK);
        wind_xmlnode_remove(xnode,child);
        err = wind_xmlnode_destroy(child);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"destroy xnode failed");
    }
    return err;
    
}



w_xmlnode_s *wind_xmlnode_get_parent(w_xmlnode_s *xnode)
{
    w_tree_s *tree;
    w_xmlnode_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    tree = xnode->tree.parent;
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

w_xmlnode_s *wind_xmlnode_get_child(w_xmlnode_s *xnode)
{
    w_tree_s *tree;
    w_xmlnode_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.child_list.head != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.child_list.head);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

w_xmlnode_s *wind_xmlnode_get_next(w_xmlnode_s *xnode)
{
    w_tree_s *tree;
    w_xmlnode_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.next != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.treenode.next);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

w_xmlnode_s *wind_xmlnode_get_prev(w_xmlnode_s *xnode)
{
    w_tree_s *tree;
    w_xmlnode_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.prev != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.treenode.prev);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

w_err_t wind_xmlnode_insert(w_xmlnode_s *parent,w_xmlnode_s *child)
{
    w_err_t err;
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_PTR_NULL);
    do
    {
        WIND_ASSERT_BREAK(parent->level < MAX_XNODE_LEVEL - 1,W_ERR_FAIL,"xnode level is too deep");
        err = wind_tree_insert_child(&parent->tree,&child->tree);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"insert child failed");
        child->level = parent->level + 1;
        
        parent->node_cnt ++;
    }while(0);
    return err;
}

w_err_t wind_xmlnode_remove(w_xmlnode_s *parent,w_xmlnode_s *child)
{
    w_err_t err;
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = wind_tree_remove_child(&parent->tree,&child->tree);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"remove child failed");
        parent->node_cnt --;
        child->level = 0;
    }while(0);
    return err;
}



w_err_t wind_xmlnode_insert_attr(w_xmlnode_s *xnode,char *attr_name,char *attr_value)
{
    w_err_t err;
    w_xmlattr_s *attr = (w_xmlattr_s *)W_NULL;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_value != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        attr = wind_malloc(sizeof(w_xmlattr_s));
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr failed");
        wind_memset(attr,0,sizeof(w_xmlattr_s));
        attr->attr_name = (char*)wind_salloc(attr_name,0);
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr name failed");
        attr->attr_value = (char*)wind_salloc(attr_value,0);
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr value failed");
        dlist_insert_tail(&xnode->attrlist,&attr->node);
        xnode->attr_cnt ++;
    }while(0);
    if(err != W_ERR_OK)
    {
        xattr_destroy(attr);
    }
    return err;
}

w_err_t    wind_xmlnode_modify_attr(w_xmlnode_s *xnode,char *attr_name,char *attr_value)
{
    w_xmlattr_s *attr;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_value != W_NULL,W_ERR_PTR_NULL);
    attr = wind_xmlnode_get_attr(xnode,attr_name);
    WIND_ASSERT_RETURN(attr != W_NULL,W_ERR_NO_OBJ);
    if(attr->attr_value != W_NULL)
        wind_free(attr->attr_value)
    attr->attr_value = wind_salloc(attr_value,0);
    WIND_ASSERT_RETURN(attr->attr_value != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}

w_xmlattr_s *wind_xmlnode_get_attr(w_xmlnode_s *xnode,char *attr_name)
{
    w_dnode_s *dnode;
    w_xmlattr_s *attr = W_NULL;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,W_NULL);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        if(wind_strcmp(attr->attr_name,attr_name) == 0)
            return attr;
    }
    return W_NULL;
}

w_err_t wind_xmlnode_delete_attr(w_xmlnode_s *xnode,char *attr_name)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_xmlattr_s *attr = W_NULL;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        foreach_node(dnode,&xnode->attrlist)
        {
            attr = NODE_TO_XATTR(dnode);
            if(wind_strcmp(attr->attr_name,attr_name) == 0)
                break;
        }
        WIND_ASSERT_BREAK(dnode != W_NULL,W_ERR_NO_OBJ,"no such a attr:%s",attr_name);
        xnode->attr_cnt --;
        dlist_remove(&xnode->attrlist,&attr->node);
        err = xattr_destroy(attr);
    }while(0);
    return err;
}




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus


