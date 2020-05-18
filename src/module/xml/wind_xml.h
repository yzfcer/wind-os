/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml.h
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
#ifndef WIND_XML_H__
#define WIND_XML_H__
#include "wind_dlist.h"
#include "wind_tree.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define MAX_XNODE_LEVEL 15
#define MAX_XNODE_CHILD 32767
#define TREE_TO_XNODE(tree) (w_xmlnode_s*)(((w_uint8_t*)(tree))-((w_addr_t)&(((w_xmlnode_s*)0)->tree)))
#define NODE_TO_XATTR(node) (w_xmlattr_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_xmlattr_s*)0)->attr_node)))

//XML node attribution struct
typedef struct
{
    w_dnode_s attr_node;//xml-attr_node attribution 
    char *attr_name;    //xml-attr_node attribution name
    char *attr_value;   //xml-attr_node attribution value
}w_xmlattr_s;



//XML node struct
typedef struct
{
    w_tree_s tree;         //xml node tree struct
    char *name;            //xml node name
    char *value;           //xml node value,sometimes null if has children
    w_uint16_t node_cnt:15;//tree child node count
    w_uint16_t is_leaf:1;  //is tree node a leaf node?
    w_uint8_t attr_cnt;    //xml node attribution count
    w_uint8_t level;       //tree node level
    w_dlist_s attrlist;    //xml node attribution list,see w_xmlattr_s
}w_xmlnode_s;

typedef struct __xml_fsm_s w_xmlfsm_s;
//xml tree infomation struct
typedef struct
{
    w_xmlnode_s *xhead;   //XMl xhead node
    w_xmlnode_s *root;    //XML root node
    w_xmlfsm_s *xfsm;
}w_xml_s;

w_err_t wind_xml_init(w_xml_s *xml);
w_err_t wind_xml_deinit(w_xml_s *xml);
w_err_t wind_xml_parse(w_xml_s *xml,char *xmlstr,w_int32_t len);
w_err_t wind_xml_print(w_xmlnode_s *xnode);

w_xmlattr_s *wind_xmlattr_create(char *attr_name,char *attr_value);
w_err_t      wind_xmlattr_destroy(w_xmlattr_s *attr);
w_xmlattr_s *wind_xmlattr_get(w_xmlnode_s *xnode,char *attr_name);
w_err_t      wind_xmlattr_modify(w_xmlattr_s *xattr,char *attr_value);

w_xmlnode_s *wind_xmlnode_create(char *name);
w_err_t      wind_xmlnode_destroy(w_xmlnode_s *xnode);
w_xmlnode_s *wind_xmlnode_get_parent(w_xmlnode_s *xnode);
w_xmlnode_s *wind_xmlnode_get_child(w_xmlnode_s *xnode);
w_xmlnode_s *wind_xmlnode_get_next(w_xmlnode_s *xnode);
w_xmlnode_s *wind_xmlnode_get_prev(w_xmlnode_s *xnode);
w_err_t      wind_xmlnode_set_value(w_xmlnode_s *xnode,char *node_value);
w_err_t      wind_xmlnode_insert(w_xmlnode_s *parent,w_xmlnode_s *child);
w_err_t      wind_xmlnode_remove(w_xmlnode_s *parent,w_xmlnode_s *child);
w_err_t      wind_xmlnode_insert_attr(w_xmlnode_s *xnode,w_xmlattr_s *xattr);
w_err_t      wind_xmlnode_remove_attr(w_xmlnode_s *xnode,w_xmlattr_s *xattr);



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_XML_H__
