w_err_t wind_xml_parse(xml_node_s *xnode,char *xmlstr,w_int32_t len)
{
    return W_ERR_FAIL;
}
w_err_t wind_xml_print(xml_node_s *xnode)
{
    return W_ERR_FAIL;
}

xml_node_s *wind_xml_get_parent(xml_node_s *xnode)
{
    w_tree_s *tree;
    xml_node_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    tree = xnode->tree.parent;
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

xml_node_s *wind_xml_get_child(xml_node_s *xnode)
{
    w_tree_s *tree;
    xml_node_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.child_list.head != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.child_list.head);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

xml_node_s *wind_xml_get_next(xml_node_s *xnode)
{
    w_tree_s *tree;
    xml_node_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.next != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.treenode.next);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

xml_node_s *wind_xml_get_prev(xml_node_s *xnode)
{
    w_tree_s *tree;
    xml_node_s *xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.prev != W_NULL,W_NULL);
    tree = NODE_TO_TREE(xnode->tree.treenode.prev);
    WIND_CHECK_RETURN(tree != W_NULL,W_NULL);
    xnode = TREE_TO_XNODE(tree);
    return xnode;
}

w_err_t wind_xml_insert_attr(xml_node_s *xnode,char *attr_name,char *attr_value)
{
    w_err_t err;
    xml_attr_s *attr = W_NULL;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_value != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        attr = wind_malloc(sizeof(xml_attr_s));
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr failed");
        wind_memset(attr,0,sizeof(xml_attr_s));
        attr->attr_name = (char*)wind_salloc(attr_name,0);
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr name failed");
        attr->attr_value = (char*)wind_salloc(attr_value,0);
        WIND_ASSERT_BREAK(attr != W_NULL,W_ERR_MEM,"alloc attr value failed");
        dlist_insert_tail(&xnode->attrlist,&attr->node);
        xnode->attr_cnt ++;
    }while(0);
    if(err != W_ERR_OK)
    {
        if(attr && attr->attr_name)
            wind_free(attr->attr_name);
        if(attr && attr->attr_value)
            wind_free(attr->attr_value);
        if(attr)
            wind_free(attr);
    }
    return err;
}

w_err_t wind_xml_remove_attr(xml_node_s *xnode,char *attr_name)
{
    w_err_t err;
    w_dnode_s *dnode;
    xml_attr_s *attr = W_NULL;
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
        if(attr && attr->attr_name)
            wind_free(attr->attr_name);
        if(attr && attr->attr_value)
            wind_free(attr->attr_value);
        if(attr)
            wind_free(attr);
    }while(0);
    
    return err;
}



