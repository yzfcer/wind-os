#ifndef XML_H__
#define XML_H__

typedef struct __xstring_s xstring_s;
typedef struct __xnode_s xnode_s;
typedef struct __xtree_s xtree_s;
struct __xstring_s
{
    char *text;
    int len;
};

struct __xnode_s
{
    xstring_s name;
    int index;
    xnode_s *parent_node;
    int level;
    int tagCount;
    xstring_s data;
};

struct __xtree_s
{
    int node_count;
    int max_node_count;
    xnode_s *pre_node;
    xnode_s *cur_node;
    xstring_s *xbuf;
    xstring_s *xbuf_pos;
    xstring_s *xbuf_tail;
    xnode_s *node_list;
    xnode_s *root_node;
};

int copy_xstring(xstring_s *dest_xs,xstring_s *src_xs);
int copy_xnode(xnode_s *dest_xnode,xnode_s *src_xnode);
xnode_s * xnode_add_brother(xtree_s *xtree,xstring_s *name);
xnode_s * xnode_add_child(xtree_s *xtree,xstring_s *name);
xnode_s *xnode_get_parent(xtree_s *xtree);
int xnode_modify_data(xtree_s *xtree,xstring_s *data);
xstring_s *delete_space(xstring_s *xbuf);
int xml_handle_attr(xtree_s *xtree,xstring_s *attr_str);
int xml_read_xnode_value(xtree_s *xtree,xstring_s *value_str);
int xml_parse(xtree_s *xtree,xstring_s *xmlbuf,int xmlsize);
int xml_parse_free(xtree_s *xtree);
int print_xtree(xtree_s *xtree);

xnode_s *xml_get_xnode_by_name(xtree_s *xtree,xstring_s *nodename);
xnode_s *xml_get_xnode(xtree_s *xtree,xstring_s *parent_name,xstring_s *nodename);
xnode_s *xml_get_child_xnode(xtree_s *xtree,xnode_s *parent_node, xstring_s *nodename);
xnode_s *xml_get_tag(xtree_s *xtree, xnode_s *parent_node,xstring_s *tagname);


int xml_encode(xstring_s *xs,xstring_s *scode,xstring_s *dcode);
int xml_data_encode(xstring_s *xs);
int xml_data_uncode(xstring_s *xs);
xstring_s *xml_write_head(xstring_s *xmlbuf,xstring_s *encode);
xstring_s *xml_write_xnode_beg(xstring_s *xmlbuf,xstring_s *tag);
xstring_s *xml_write_xnode_end(xstring_s *xmlbuf,xstring_s *tag);
xstring_s *xml_write_tag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data);
xstring_s *xml_write_extag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data);
xstring_s *xml_write_note(xstring_s *xmlbuf,xstring_s *note);
xstring_s *xml_write_buf(xstring_s *buf,xstring_s *str);


#endif
