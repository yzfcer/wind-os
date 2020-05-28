/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : wind_xml.c
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2020/03/24
** Description : 
** Others      : An external mutex is required when used in a multithreaded environment,
** Modify      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#include "wind_xml.h"
#include "wind_xml_fsm.h"
#include "wind_debug.h"
#include "wind_macro.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#define NODE_TO_XNODE(node) (w_xmlnode_s *)(((w_uint8_t*)(node))-((w_addr_t)&(((w_xmlnode_s*)0)->tree.treenode)))
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

w_xmlattr_s *wind_xmlattr_create(char *attr_name,char *attr_value)
{
    w_err_t err;
    w_xmlattr_s *xattr = (w_xmlattr_s *)W_NULL;
    WIND_ASSERT_RETURN(attr_name != W_NULL,(w_xmlattr_s *)W_NULL);
    WIND_ASSERT_RETURN(attr_value != W_NULL,(w_xmlattr_s *)W_NULL);
    do
    {
    
        err = W_ERR_OK;
        xattr = (w_xmlattr_s *)wind_malloc(sizeof(w_xmlattr_s));
        WIND_ASSERT_BREAK(xattr != W_NULL,W_ERR_MEM,"alloc xattr failed");
        wind_memset(xattr,0,sizeof(w_xmlattr_s));
        xattr->attr_name = (char*)wind_salloc(attr_name,0);
        WIND_ASSERT_BREAK(xattr != W_NULL,W_ERR_MEM,"alloc xattr name failed");
        xattr->attr_value = (char*)wind_salloc(attr_value,0);
        WIND_ASSERT_BREAK(xattr != W_NULL,W_ERR_MEM,"alloc xattr value failed");
    }while(0);
    if(err != W_ERR_OK)
    {
        wind_xmlattr_destroy(xattr);
        xattr = (w_xmlattr_s *)W_NULL;
    }
    return xattr;
}

w_err_t wind_xmlattr_destroy(w_xmlattr_s *attr)
{
    WIND_CHECK_RETURN(attr != W_NULL,W_ERR_PTR_NULL);
    if(attr->attr_name)
        wind_free(attr->attr_name);
    if(attr->attr_value)
        wind_free(attr->attr_value);
    wind_free(attr);
    return W_ERR_OK;
}

w_err_t    wind_xmlattr_modify(w_xmlattr_s *xattr,char *attr_value)
{
    WIND_ASSERT_RETURN(xattr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(attr_value != W_NULL,W_ERR_PTR_NULL);
    if(xattr->attr_value != W_NULL)
        wind_free(xattr->attr_value);
    xattr->attr_value = (char*)wind_salloc(attr_value,0);
    WIND_ASSERT_RETURN(xattr->attr_value != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}

w_xmlattr_s *wind_xmlattr_get(w_xmlnode_s *xnode,char *attr_name)
{
    w_dnode_s *dnode;
    w_xmlattr_s *attr = (w_xmlattr_s *)W_NULL;
    WIND_ASSERT_RETURN(xnode != W_NULL,(w_xmlattr_s *)W_NULL);
    WIND_ASSERT_RETURN(attr_name != W_NULL,(w_xmlattr_s *)W_NULL);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        if(wind_strcmp(attr->attr_name,attr_name) == 0)
            return attr;
    }
    return (w_xmlattr_s *)W_NULL;
}


w_err_t wind_xml_init(w_xml_s *xml)
{
    w_err_t err;
    WIND_ASSERT_RETURN(xml != W_NULL,W_ERR_PTR_NULL);
    wind_memset(xml,0,sizeof(w_xml_s));
    xml->xfsm = (w_xmlfsm_s*)wind_malloc(sizeof(w_xmlfsm_s));
    err = wind_xml_fsm_init(xml->xfsm,"xml");
    return err;
}

w_err_t wind_xml_deinit(w_xml_s *xml)
{
    WIND_ASSERT_RETURN(xml != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xml->xfsm != W_NULL,W_ERR_PTR_NULL);
    //if(xml->version)
    //    wind_xmlnode_destroy(xml->version);
    //if(xml->root)
    //    wind_xmlnode_destroy(xml->root);
    if(xml->xfsm)
    {
        wind_xml_fsm_deinit(xml->xfsm);
        wind_free(xml->xfsm);
    }
        
    return W_ERR_OK;
}


w_err_t wind_xml_parse(w_xml_s *xml,char *xmlstr,w_int32_t len)
{
    w_err_t err;
    WIND_ASSERT_RETURN(xml != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xmlstr != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len > 0,W_ERR_OVERFLOW);
    err = wind_xml_fsm_input(xml->xfsm,xmlstr,len);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    if(xml->xfsm->complete_flag)
    {
        xml->xhead = xml->xfsm->xhead;
        xml->root = xml->xfsm->root;
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

static void print_align(w_xmlnode_s *xnode)
{
    w_int32_t i;
    char prefix[MAX_XNODE_LEVEL+1];
    wind_memset(prefix,0,sizeof(prefix));
    for(i = 0;i < xnode->level;i ++)
        prefix[i] = '\t';
    wind_printf(prefix);
}

#if 0
w_err_t wind_xml_print(w_xmlnode_s *xnode)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_xmlattr_s *attr;
    //w_tree_s *tree;
    w_xmlnode_s *tmp_xndoe;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->level < MAX_XNODE_LEVEL,W_ERR_FAIL);
    err = W_ERR_OK;
    print_align(xnode);
    wind_printf("<%s",xnode->name);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        wind_printf(" %s=\"%s\"",attr->attr_name,attr->attr_value);
    }
    if(xnode->is_leaf && xnode->value == W_NULL)
        wind_printf("/>\r\n");
    else
        wind_printf(">");
    if(xnode->is_leaf  && xnode->value != W_NULL)
        wind_printf("%s",xnode->value);
    else if(dlist_head(&xnode->tree.child_list))
    {
        wind_printf("\r\n");
        foreach_node(dnode,&xnode->tree.child_list)
        {
            tmp_xndoe = NODE_TO_XNODE(dnode);
            err = wind_xml_print(tmp_xndoe);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"print child xnode failed");
        }
        //wind_printf("\r\n");
    }
    if(!xnode->is_leaf || xnode->value != W_NULL)
    {
        if(!xnode->is_leaf)
            print_align(xnode);
        wind_printf("</%s>\r\n",xnode->name);
    }
    return W_ERR_OK;
}
#else
w_err_t wind_xml_print(w_xmlnode_s *xnode)
{
    return wind_xmlnode_to_string(xnode,(w_xmlout_fn)wind_std_output);
}
#endif

w_xmlnode_s *wind_xmlnode_create(char *name)
{
    w_err_t err;
    w_bool_t isvalid;
    w_xmlnode_s *xnode = (w_xmlnode_s *)W_NULL;
    do
    {
        err = W_ERR_OK;
        isvalid = is_xml_name_valid(name);
        WIND_ASSERT_BREAK(isvalid == W_TRUE,W_ERR_INVALID,"node name is invalid");
        xnode = (w_xmlnode_s *)wind_malloc(sizeof(w_xmlnode_s));
        wind_memset(xnode,0,sizeof(w_xmlnode_s));
        xnode->name = (char*)wind_salloc(name,0);
        WIND_ASSERT_BREAK(xnode->name != W_NULL,W_ERR_MEM,"alloc xnode name failed");
        xnode->is_leaf = 1;
    }while(0);
    if(err != W_ERR_OK)
    {
        wind_xmlnode_destroy(xnode);
        xnode = (w_xmlnode_s *)W_NULL;
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
    xnode->name = (char*)W_NULL;
    xnode->value= (char*)W_NULL;
    
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        wind_xmlnode_remove_attr(xnode,attr);
        wind_xmlattr_destroy(attr);
    }
    DLIST_INIT(xnode->attrlist);
    err = W_ERR_OK;
    for(;;)
    {
        child = wind_xmlnode_get_child(xnode);
        WIND_CHECK_BREAK(child != W_NULL,W_ERR_OK);
        wind_xmlnode_remove(xnode,child);
        err = wind_xmlnode_destroy(child);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"destroy xnode failed");
    }
    wind_tree_init(&xnode->tree);
    wind_free(xnode);
    return err;
    
}



w_xmlnode_s *wind_xmlnode_get_parent(w_xmlnode_s *xnode)
{
    w_dnode_s *dnode;
    w_xmlnode_s *tmp_xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,(w_xmlnode_s *)W_NULL);
    WIND_CHECK_RETURN(xnode->tree.parent != W_NULL,(w_xmlnode_s *)W_NULL);
    dnode = &xnode->tree.parent->treenode;
    tmp_xnode = NODE_TO_XNODE(dnode);
    return tmp_xnode;
}

w_xmlnode_s *wind_xmlnode_get_child(w_xmlnode_s *xnode)
{
    w_dnode_s *dnode;
    w_xmlnode_s *tmp_xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,(w_xmlnode_s *)W_NULL);
    WIND_CHECK_RETURN(xnode->tree.child_list.head != W_NULL,(w_xmlnode_s *)W_NULL);
    dnode = xnode->tree.child_list.head;
    tmp_xnode = NODE_TO_XNODE(dnode);
    return tmp_xnode;
}

w_xmlnode_s *wind_xmlnode_get_next(w_xmlnode_s *xnode)
{
    w_dnode_s *dnode;
    w_xmlnode_s *tmp_xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,(w_xmlnode_s *)W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.next != W_NULL,(w_xmlnode_s *)W_NULL);
    dnode = xnode->tree.treenode.next;
    tmp_xnode = NODE_TO_XNODE(dnode);
    return tmp_xnode;
}

w_xmlnode_s *wind_xmlnode_get_prev(w_xmlnode_s *xnode)
{
    w_dnode_s *dnode;
    w_xmlnode_s *tmp_xnode;
    WIND_CHECK_RETURN(xnode != W_NULL,(w_xmlnode_s *)W_NULL);
    WIND_CHECK_RETURN(xnode->tree.treenode.prev != W_NULL,(w_xmlnode_s *)W_NULL);
    dnode = xnode->tree.treenode.prev;
    tmp_xnode = NODE_TO_XNODE(dnode);
    return tmp_xnode;
}

w_err_t wind_xmlnode_set_value(w_xmlnode_s *xnode,char *node_value)
{
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->is_leaf,W_ERR_INVALID);
    if(xnode->value != W_NULL)
        wind_free(xnode->value);
    xnode->value = (char*)W_NULL;
    if(node_value != W_NULL)
        xnode->value = (char*)wind_salloc(node_value,0);
    return W_ERR_OK;
}

w_err_t wind_xmlnode_insert(w_xmlnode_s *parent,w_xmlnode_s *child)
{
    w_err_t err;
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(parent->value == W_NULL,W_ERR_INVALID);
    do
    {
        WIND_ASSERT_BREAK(parent->level < MAX_XNODE_LEVEL - 1,W_ERR_FAIL,"xnode level is too deep");
        err = wind_tree_insert_child(&parent->tree,&child->tree);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"insert child failed");
        child->level = parent->level + 1;
        WIND_ASSERT_BREAK(parent->node_cnt < MAX_XNODE_CHILD,W_ERR_FAIL,"child node is too many");
        parent->node_cnt ++;
        parent->is_leaf = 0;
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
        WIND_ASSERT_BREAK(parent->node_cnt > 0,W_ERR_FAIL,"child node count error");
        parent->node_cnt --;
        child->level = 0;
        if(parent->node_cnt == 0)
            parent->is_leaf = 1;
    }while(0);
    return err;
}



w_err_t wind_xmlnode_insert_attr(w_xmlnode_s *xnode,w_xmlattr_s *xattr)
{
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xattr != W_NULL,W_ERR_PTR_NULL);
    dlist_insert_tail(&xnode->attrlist,&xattr->attr_node);
    WIND_ASSERT_RETURN(xnode->attr_cnt < 255,W_ERR_FAIL);
    xnode->attr_cnt ++;
    return W_ERR_OK;
}


w_err_t wind_xmlnode_remove_attr(w_xmlnode_s *xnode,w_xmlattr_s *xattr)
{

    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xattr != W_NULL,W_ERR_PTR_NULL);

    WIND_ASSERT_RETURN(xnode->attr_cnt > 0,W_ERR_FAIL);
    xnode->attr_cnt --;
    dlist_remove(&xnode->attrlist,&xattr->attr_node);

    return W_ERR_OK;
}


w_err_t      wind_xmlnode_to_string(w_xmlnode_s *xnode,w_xmlout_fn xmlout)
{
    w_err_t err;
    char *buff;
    w_int32_t len;
    w_dnode_s *dnode;
    w_xmlattr_s *attr;
    w_xmlnode_s *tmp_xndoe;
    WIND_ASSERT_RETURN(xnode != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(xnode->level < MAX_XNODE_LEVEL,W_ERR_FAIL);
    buff = (char*)wind_malloc(XML_OUTBUF_LEN);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
    
    err = W_ERR_OK;
    print_align(xnode);
    len = wind_sprintf(buff,"<%s",xnode->name);
    xmlout(buff,len);
    foreach_node(dnode,&xnode->attrlist)
    {
        attr = NODE_TO_XATTR(dnode);
        len = wind_sprintf(buff," %s=\"%s\"",attr->attr_name,attr->attr_value);
        xmlout(buff,len);
    }
    if(xnode->is_leaf && xnode->value == W_NULL)
    {
        len = wind_sprintf(buff,"/>\r\n");
        xmlout(buff,len);
    }

    else
    {
        len = wind_sprintf(buff,">");
        xmlout(buff,len);
    }
        
    if(xnode->is_leaf  && xnode->value != W_NULL)
    {
        len = wind_sprintf(buff,"%s",xnode->value);
        xmlout(buff,len);
    }
    else if(dlist_head(&xnode->tree.child_list))
    {
        len = wind_sprintf(buff,"\r\n");
        xmlout(buff,len);
        foreach_node(dnode,&xnode->tree.child_list)
        {
            tmp_xndoe = NODE_TO_XNODE(dnode);
            err = wind_xml_print(tmp_xndoe);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"print child xnode failed");
        }
    }
    if(!xnode->is_leaf || xnode->value != W_NULL)
    {
        if(!xnode->is_leaf)
            print_align(xnode);
        len = wind_sprintf(buff,"</%s>\r\n",xnode->name);
        xmlout(buff,len);
    }
    wind_free(buff);
    return W_ERR_OK;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus


