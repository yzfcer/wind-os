#ifndef WIND_XML_H__
#define WIND_XML_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define TREE_TO_XNODE(tree) (xml_node_s*)(((w_uint8_t*)(tree))-((w_addr_t)&(((xml_node_s*)0)->tree)))
#define NODE_TO_XATTR(node) (xml_attr_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((xml_attr_s*)0)->node)))

//XML attribution struct
typedef struct
{
    w_dnode_s node;
    char *attr_name;
    char *attr_value;
}xml_attr_s;



//XML content node struct
typedef struct
{
    w_tree_s tree;
    char *name;
    char *value;
    w_int16_t attr_cnt;
    w_int16_t level;
    w_dlist_s attrlist;
    //w_dnode_s attrnode;
}xml_node_s;



typedef struct
{
    
    xml_node_s *root;
    
}xml_root_s;

w_err_t wind_xml_parse(xml_node_s *xnode,char *xmlstr,w_int32_t len);
w_err_t wind_xml_print(xml_node_s *xnode);

xml_node_s *wind_xml_get_parent(xml_node_s *xnode);
xml_node_s *wind_xml_get_child(xml_node_s *xnode);
xml_node_s *wind_xml_get_next(xml_node_s *xnode);
xml_node_s *wind_xml_get_prev(xml_node_s *xnode);

w_err_t wind_xml_insert_attr(xml_node_s *xnode,char *attr_name,char *attr_value);
w_err_t wind_xml_remove_attr(xml_node_s *xnode,char *attr_name);


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_XML_H__
