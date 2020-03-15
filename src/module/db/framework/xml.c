#include "wind_string.h"
#include "wind_heap.h"
#include"xml.h"
#include"wind_debug.h"

/*xml�õ��ڵ���ֵ,����:1�ڵ��ʶ����ͷ,2�ڵ��ʶ����β,3ע��,4Ԫ������  <= 0�д��� */ 
#define XVAL_NBEGIN 1 
#define XVAL_NEND 2 
#define XVAL_NOTE 3 
#define XVAL_DATA 4 
#define XVAL_TAG 5  /*�ձ�־ */ 
#define XVAL_NONE 0 
#define XVAL_ERROR -1 

/*�ж��Ƿ�ո������xml�ָ��ַ� */ 
static int is_space(char c) 
{ 
    switch(c)
    { 
        case 0x20: 
        case 0xD: 
        case 0xA: 
        case 0x9: 
            return 1;
    }
    return 0;
}

/*�ж��Ƿ���Ч������ǰ���� */ 
static int is_valid_prefix(int c) 
{ 
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_'); 
}

/*XML�ַ�����ֵ */ 
int copy_xstring(xstring_s *dest_xs,xstring_s *src_xs) 
{ 
    dest_xs->text = src_xs->text;
    dest_xs->len = src_xs->len;
    return 0;
}

/*XML�ڵ㸳ֵ */ 
int copy_xnode(xnode_s *dest_xnode,xnode_s *src_xnode) 
{ 
    copy_xstring(&(dest_xnode->name),&(src_xnode->name));
    dest_xnode->index = src_xnode->index;
    dest_xnode->parent_node = src_xnode->parent_node;
    dest_xnode->level = src_xnode->level;
    dest_xnode->tagCount = src_xnode->tagCount;
    copy_xstring(&(dest_xnode->data),&(src_xnode->data)); 
    return 0;
}

/*����ͬ���ڵ�: */ 
xnode_s *xnode_add_brother(xtree_s *xtree,xstring_s *name) 
{
    if(xtree->cur_node->parent_node == W_NULL || xtree->node_count >= xtree->max_node_count)
        return W_NULL;
    (xtree->node_count) ++ ;
    xtree->pre_node = xtree->cur_node;
    xtree->cur_node = &(xtree->node_list[xtree->node_count-1]);
    xtree->cur_node->index = xtree->node_count-1;
    copy_xstring(&(xtree->cur_node->name),name);
    xtree->cur_node->parent_node = xtree->pre_node->parent_node;
    xtree->cur_node->level = xtree->pre_node->level;
    (xtree->cur_node->parent_node->tagCount) ++ ;
    return (xtree->cur_node);
}

/*�����ӽڵ�: */ 
xnode_s *xnode_add_child(xtree_s *xtree,xstring_s *name) 
{ 
    if(xtree->cur_node == W_NULL || xtree->node_count >= xtree->max_node_count)
        return W_NULL;
    (xtree->node_count) ++ ;
    xtree->pre_node = xtree->cur_node;
    xtree->cur_node = &(xtree->node_list[xtree->node_count-1]);
    xtree->cur_node->index = xtree->node_count-1;
    copy_xstring(&(xtree->cur_node->name),name);
    xtree->cur_node->parent_node = xtree->pre_node;
    xtree->cur_node->level = xtree->pre_node->level + 1;
    (xtree->pre_node->tagCount) ++ ;
    return (xtree->cur_node);
}

/*���ظ��ڵ�: */ 
xnode_s *xnode_get_parent(xtree_s *xtree) 
{ 
    if(xtree->cur_node->parent_node == W_NULL)
        return W_NULL;
    xtree->pre_node = xtree->cur_node;
    xtree->cur_node = xtree->cur_node->parent_node;
    return (xtree->cur_node);
}

/*�޸ĵ�ǰ�ڵ�Ԫ��ֵ: */ 
int xnode_modify_data(xtree_s *xtree,xstring_s *data) 
{ 
    if(xtree->cur_node == W_NULL)
        return -1;
    /*if(xtree->cur_node->tagCount > 0)
        return -2; */ 
    copy_xstring(&(xtree->cur_node->data),data); 
    return 0;
}

/*ɾ��ǰ�����ַ� */ 
xstring_s *delete_prefix_space(xstring_s *xbuf) 
{ 
    xstring_s *p;
    for(p = xbuf;is_space(*(char*)p);p ++);
    return (p);
}

/*��������:����Ҷ�ڵ�,�ӽڵ���Ϊ-1,�������ӵ����Խڵ��� */ 
int xml_handle_attr(xtree_s *xtree,xstring_s *attr_str) 
{ 
    xstring_s xn,xv;
    int k = 0;
    xstring_s *p1, *p, *p2;
    p = delete_prefix_space((xstring_s*)attr_str->text);
    while(p && *(char*)p)
    { 
        p1 = p;
        p2 = (xstring_s*)wind_strchr((char*)p1,'=');
        if(p2 == W_NULL)
            break;
        xn.text = (char*)p1;
        *(char*)p2 = '\0';
        xn.len = p2-p1;
        p = delete_prefix_space(p2 + 1);
        if(*(char*)p != '\"')
            break;
        p1 = p;
        p2 = (xstring_s*)wind_strchr((char*)(p1 + 1),'\"');
        if(p2 == W_NULL)
            break;
        xv.text = (char*)(p1 + 1);
        *(char*)p2 = '\0';
        xv.len = p2-p1-1;
        /*��������ֵ�ڵ� */ 
        if(xnode_add_child(xtree,&xn) == W_NULL)
            break;
        k ++ ;
        if(xnode_modify_data(xtree,&xv) < 0)
            break;
        xtree->cur_node->tagCount = -1;
        xnode_get_parent(xtree);
        p = delete_prefix_space(p2 + 1);
    }
    return (k);
}

/*xml�õ��ڵ���ֵ,����:1�ڵ��ʶ����ͷ,2�ڵ��ʶ����β,3ע��,4Ԫ������ 0�� -1�д��� */ 
int xml_read_xnode_value(xtree_s *xtree,xstring_s *value_str) 
{ 
    xstring_s *p, *p1, *p2;
    int ivtype = XVAL_NONE;
    p = xtree->xbuf_pos;
    p = delete_prefix_space(xtree->xbuf_pos);
    if(*(char*)p!= '<')
    {  /*���� */ 
        p1 = p;
        p2 = (xstring_s*)wind_strchr((char*)p1,'<');
        value_str->text = (char*)p1;
        value_str->len = (p2?p2-p1:0);
        ivtype = (p2 && p2 < xtree->xbuf_tail? XVAL_DATA:XVAL_NONE);
        xtree->xbuf_pos = (p2?p2:xtree->xbuf_tail);
        goto OKExit33qqq;
    }

    /*ǰ��Ϊ < */ 
    p ++ ;
    if(*(char*)p == '/') 
    { /*���β */ 
        p1 = p + 1;
        p2 = (xstring_s*)wind_strchr((char*)p1,'>');
        if(p2 == W_NULL)
        { /*���� */ 
            goto ErrExit33qqq;
        }
        else
        { 
            ivtype = XVAL_NEND;
            value_str->text = (char*)p1;
            value_str->len = p2-p1;
            xtree->xbuf_pos = p2 + 1;
        }
    }
    else if(is_valid_prefix(*(char*)p))
    { /*���ͷ */ 
        p1 = p;
        p2 = (xstring_s*)wind_strchr((char*)p1,'>');
        if(p2 == W_NULL)
        { /*���� */ 
            goto ErrExit33qqq;
        } 
        for(p = p1; *(char*)p!= ' ' && p < p2;p ++);
        if(*((char*)(p2-1)) == '/')
        { /*�����ձ�� < abc/ >  */ 
            ivtype = 11;
            value_str->text = (char*)p1;
            if(p >= p2)
                p = p2-1;
            value_str->len = p-p1;
            if(xnode_add_child(xtree,value_str) == W_NULL)
                goto ErrExit33qqq;
            if(p < p2-1)/*�������Խڵ� */ 
            { 
                *((char*)(p2-1)) = '\0';
                p ++ ;
                value_str->text = (char*)p;
                value_str->len = p2-1-p;
                ivtype += xml_handle_attr(xtree,value_str);
            }
            xnode_get_parent(xtree);
        }
        else
        { 
            ivtype = XVAL_NBEGIN;
            value_str->text = (char*)p1;
            value_str->len = p-p1;
            if(p < p2) /*�������Խڵ� */ 
            { 
                if(xnode_add_child(xtree,value_str) == W_NULL)
                    goto ErrExit33qqq;
                *((char*)p2) = '\0';
                p ++ ;
                value_str->text = (char*)p;
                value_str->len = p2-p;
                ivtype = 11 + xml_handle_attr(xtree,value_str);
            }  
        }
        xtree->xbuf_pos = p2 + 1;
    }
    else if(*(char*)p == '!')
    {  
        if(*((char*)p + 1) == '-' &&  *((char*)p + 2) == '-') /*ע�� */ 
        { 
            p += 3; 
            p1 = delete_prefix_space(p);
            p2 = (xstring_s*)wind_strstr((char*)p1,"-->"); /*ע��β */ 
            if(p2 == W_NULL)
            { /*���� */ 
                goto ErrExit33qqq;
            }
            else
            { 
                ivtype = XVAL_NOTE;
                xtree->xbuf_pos = p2 + 3;
                value_str->text = (char*)p1;
                value_str->len = (p2-p1)/sizeof(xstring_s);
            }
            goto OKExit33qqq;
        }
    }

OKExit33qqq: 
    return (ivtype);
ErrExit33qqq: 
    ivtype = XVAL_ERROR;
    xtree->xbuf_pos = xtree->xbuf_tail;
    return (ivtype); 
}

/*��xml�ļ����沢������X�� */ 
int xml_parse(xtree_s *xtree,xstring_s *xmlbuf,int xmlsize) 
{ 
    xstring_s *p, *p1, *pend = xmlbuf + xmlsize;
    xstring_s xstr;
    xnode_s *xn;
    int k;
    int maxnn = 0,vtype = XVAL_NONE, vtype0 = XVAL_NONE;

    wind_memset(xtree,0x00,sizeof(xtree_s));
    if(wind_memcmp(xmlbuf,"<?xmlversion=",14)!= 0) 
    { 
        wind_error("xml has no content!");
        return -1;
    }
    p = (xstring_s*)wind_strstr((char*)xmlbuf,"?>");
    if(p == W_NULL) 
    { 
        wind_error("XML head has no tail!");
        return -1;
    }
    p1 = delete_prefix_space(p + 2);
    for(p = p1,maxnn = 1;p < pend;p ++)
    { 
        if(*(char*)p == '>' ||  *(char*)p == '"')
            maxnn ++ ;
        if(*(char*)p == '>')
            if(*((char*)p-1) == '/') 
            maxnn ++ ;/*�ձ�� */ 
    }
    maxnn = maxnn/2 + 2;
    xtree->xbuf = xmlbuf;
    xtree->xbuf_tail = xmlbuf + xmlsize;
    xtree->xbuf_pos = p1;
    for(k = 0;k < 99;k ++)
    { 
        vtype = xml_read_xnode_value(xtree,&xstr); 
        if(vtype == XVAL_NBEGIN)
            break;
        else if(vtype!= XVAL_NOTE)
            break;
    }

    xtree->node_count = 0;
    if (vtype!= XVAL_NBEGIN) 
    { 
        xtree->max_node_count = 0;
        wind_debug("xml content is NULL");
        return 0;
    }
    wind_debug("alloc node space(%d)",maxnn);
    xtree->node_list = (xnode_s *)wind_calloc(maxnn,sizeof(xnode_s)); /*����ڵ�洢�ռ� */ 
    if(xtree->node_list == W_NULL) 
    { 
        wind_error("mem %s alloc failed",maxnn);
        return -2;
    }
    xtree->max_node_count = maxnn;
    wind_memset(xtree->node_list,0x00,maxnn *sizeof(xnode_s));
    xtree->root_node = &(xtree->node_list[0]); /*���ڵ� */ 
    xtree->root_node->parent_node = W_NULL;
    xtree->root_node->index = xtree->root_node->level = 0;
    *(xstr.text + xstr.len) = '\0';
    copy_xstring(&(xtree->root_node->name),&xstr);
    xtree->cur_node = xtree->root_node;
    xtree->node_count = 1;
    k = 0;
    while(k < maxnn) 
    { 
        vtype0 = vtype;
        vtype = xml_read_xnode_value(xtree,&xstr);
        if(vtype <= XVAL_NONE)
            break;
        switch(vtype)
        { 
        case XVAL_NOTE: 
            vtype = vtype0;
            break;
        case XVAL_NBEGIN: 
            if(xnode_add_child(xtree,&xstr) == W_NULL)
                k = maxnn + 1; 
            break;
        case XVAL_NEND: 
            xnode_get_parent(xtree); /*���ظ��ڵ�: */ 
            break;
        case XVAL_DATA: 
            xnode_modify_data(xtree,&xstr);/*�޸ĵ�ǰ�ڵ�Ԫ��ֵ: */ 
            break;
        default: 
            if(vtype > 10) 
                break;
        }
    }
    *(char*)xmlbuf = '\0'; /*���ѽ�����־ */ 
    maxnn = xtree->cur_node->index; /*���յ�ǰ�ڵ�� */ 
    for(k = 0;k < xtree->node_count;k ++)
    { 
        xn = &(xtree->node_list[k]);
        if(xn->name.text) 
           *(xn->name.text + xn->name.len) = '\0';
        else 
            xn->name.text = (char*)xmlbuf;
        if(xn->data.text)
           *(xn->data.text + xn->data.len) = '\0';
        else 
            xn->data.text = (char*)xmlbuf; 
    }
    wind_debug ("node_count = %d/n",xtree->node_count);
    if(maxnn)
        wind_error("XML file is invalid(%d)",maxnn);
    xtree->cur_node = xtree->root_node; 
    return (maxnn);
}

int xml_parse_free(xtree_s *xtree) 
{ 
    if(xtree->max_node_count > 0) 
        wind_free(xtree->node_list);
    xtree->max_node_count = 0;
    xtree->node_count = 0;
    return 0;
}

int print_xtree(xtree_s *xtree) 
{ 
    int k;
    xnode_s *xn;
    wind_printf(" ********xTree node_count=%d,max_node_count=%d ********",xtree->node_count,xtree->max_node_count);
    wind_printf("/tnode_idx parent node_level sub_count node_name node_data");
    for(k = 0;k < xtree->node_count;k ++)
    { 
        xn = &(xtree->node_list[k]);
        wind_printf("/t%d%d%d%d[%s][%s]",xn->index,(xn->parent_node?xn->parent_node->index:-1), xn->level,xn->tagCount,xn->name.text,xn->data.text);
    }
    wind_printf(" *****************************************************************");
    return 0;
}

/*�ӵ�ǰ�ڵ�����ȡ�ýڵ�: */ 
xnode_s *xml_get_xnode_by_name(xtree_s *xtree,xstring_s *nodename) 
{ 
    int k;
    xnode_s *xn;
    for(k = xtree->cur_node->index + 1;k < xtree->node_count;k ++) 
    { 
        xn = &(xtree->node_list[k]);
        if(wind_memcmp(xn->name.text,nodename,xn->name.len) == 0)
            break;
    }
    if(k >= xtree->node_count)
        return W_NULL;
    xtree->cur_node = xn;
    return (xn);
}

/*�ӵ�ǰ�ڵ�����ȡ���ӽڵ�(���ݸ��ڵ����ͽڵ���): */ 
xnode_s *xml_get_xnode(xtree_s *xtree,xstring_s *parent_name,xstring_s *nodename) 
{ 
     int k;
    xnode_s *xn;
    for(k = xtree->cur_node->index + 1;k < xtree->node_count;k ++) 
    { 
        xn = &(xtree->node_list[k]);
        if(wind_memcmp(xn->name.text,nodename,xn->name.len) == 0 && wind_memcmp(xn->parent_node->name.text,parent_name,xn->parent_node->name.len) == 0)
            break;
    }
    if(k >= xtree->node_count)
        return W_NULL;
    xtree->cur_node = xn;
    return (xtree->cur_node);
}

/*�ӵ�ǰ�ڵ�����ȡ���ӽڵ�(���ݸ��ڵ�ͽڵ���): */ 
xnode_s *xml_get_child_xnode(xtree_s *xtree,xnode_s *parent_node, xstring_s *nodename) 
{ 
    int k;
    xnode_s *xn;
    for(k = xtree->cur_node->index + 1;k < xtree->node_count;k ++) 
    { 
        xn = &(xtree->node_list[k]);
        if(xn->level <= parent_node->level)
        return W_NULL;
        if(xn->parent_node == parent_node && wind_memcmp(xn->name.text,nodename,xn->name.len) == 0)
            break;
    }
    if(k >= xtree->node_count)
        return W_NULL;
    xtree->cur_node = xn;
    return (xtree->cur_node);
}

/*ȡ��Ҷ�ڵ�: */ 
xnode_s *xml_get_tag(xtree_s *xtree, xnode_s *parent_node,xstring_s *tagname) 
{ 
     int k;
    xnode_s *xn;
    if(parent_node == W_NULL)
        return W_NULL;
    for(k = parent_node->index + 1;k < xtree->node_count;k ++) 
    { 
        xn = &(xtree->node_list[k]);
        if(xn->level <= parent_node->level)
        return W_NULL;
        if(xn->parent_node == parent_node && wind_memcmp(xn->name.text,tagname,xn->name.len) == 0)
            break;
    }
    if(k >= xtree->node_count)
        return W_NULL;
    xtree->cur_node = xn;
    return (xtree->cur_node);
}

/*�������ת��1: */ 
int xml_encode(xstring_s *xs,xstring_s *scode,xstring_s *dcode) 
{ 
    int k,lens = wind_strlen((char*)scode),lend = wind_strlen((char*)dcode),nrr = 0;
    xstring_s *tmpbuf, *p, *p0, *pt, *pend;
    if(wind_strstr(xs->text,(char*)scode) == W_NULL) 
        return 0;
    pend = (xstring_s *)(xs->text + xs->len);
    k = lend-lens;
    if(k < 0)
        k = 0;
    tmpbuf = (xstring_s *)wind_malloc(xs->len + k *100 + 1);
    if(tmpbuf == W_NULL)
        return -1;
    wind_memset(tmpbuf,0x00,xs->len + k *100);
    for(p0 = p = (xstring_s*)xs->text,pt = tmpbuf;p && nrr < 100;nrr ++)
    { 
        p = (xstring_s*)wind_strstr((char*)p0,(char*)scode);
        if(p == W_NULL || p > pend)
        { 
            wind_memcpy(pt,p0,pend-p0);
            pt += pend-p0;
            break;
        }
        wind_memcpy(pt,p0,p-p0);pt += p-p0;
        wind_memcpy(pt,dcode,lend);pt += lend;
        p0 = p + lens;
    }
    pt = '\0';
    lens = wind_strlen((char*)tmpbuf);
    wind_memcpy(xs->text,tmpbuf,lens);
    xs->len = lens;
    *(xs->text + lens) = '\0';
    wind_free(tmpbuf);
    return (nrr);
}

/*XML�������ת��: */ 
int xml_data_encode(xstring_s *xs) 
{ 
    xml_encode(xs,(xstring_s*)"<",(xstring_s*)"<");
    xml_encode(xs,(xstring_s*)">",(xstring_s*)">");
    xml_encode(xs,(xstring_s*)"/\"",(xstring_s*)"\"");
    xml_encode(xs,(xstring_s*)"'",(xstring_s*)"'");
    xml_encode(xs,(xstring_s*)"&",(xstring_s*)"&");
    /*Ϊ��ֹ���зָ�����ҵ��ϵͳ��ͻ,�Ǳ�׼ת�� */ 
    xml_encode(xs,(xstring_s*)"|",(xstring_s*)":");
    return (0);
}

/*XML������ŷ�ת��: */ 
int xml_data_uncode(xstring_s *xs) 
{ 
    xml_encode(xs,(xstring_s*)"&",(xstring_s*)"&");
    xml_encode(xs,(xstring_s*)"<",(xstring_s*)"<");
    xml_encode(xs,(xstring_s*)">",(xstring_s*)">");
    xml_encode(xs,(xstring_s*)"/\"",(xstring_s*)"\"");
    xml_encode(xs,(xstring_s*)"'",(xstring_s*)"'");
    return (0);
}

/*дxmlͷ */ 
xstring_s *xml_write_head(xstring_s *xmlbuf,xstring_s *encode) 
{ 
    wind_sprintf((char*)xmlbuf,"<?xmlversion=\"1.0\"encoding=\"%s\"?>\n\0",(char*)encode);
    return (xmlbuf + wind_strlen((char*)xmlbuf));
}

/*д��ʶͷ */ 
xstring_s *xml_write_xnode_beg(xstring_s *xmlbuf,xstring_s *tag) 
{ 
    int k;
    if(*(char*)tag > '9')
    { 
        wind_sprintf((char*)xmlbuf,"<%s>\n\0",(char*)tag);
        return (xmlbuf + wind_strlen((char*)tag) + 3);
    }
    else
    { 
        for(k = 0;k < (*(char*)tag-'0');k ++)
            *((char*)xmlbuf + k) = '\t';
        wind_sprintf((char*)xmlbuf + k,"<%s>\n\0",tag + 1);
        return (xmlbuf + k + wind_strlen((char*)tag) + 2);
    }
}

/*д��ʶβ */ 
xstring_s *xml_write_xnode_end(xstring_s *xmlbuf,xstring_s *tag) 
{ 
    int k;
    if(*(char*)tag > '9')
    { 
        wind_sprintf((char*)xmlbuf,"</%s>\n\0",(char*)tag);
        return (xmlbuf + wind_strlen((char*)tag) + 4);
    }
    else
    { 
        for(k = 0;k < (*(char*)tag-'0');k ++) 
           *((char*)xmlbuf + k) = '\t';
        wind_sprintf((char*)xmlbuf + k,"</%s>\n\0",(char*)tag + 1);
        return (xmlbuf + k + wind_strlen((char*)tag) + 3);
    } 
}

/*д����Ԫ�� */ 
xstring_s *xml_write_tag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data) 
{ 
    int k;
    if(*(char*)tag > '9')
    { 
        wind_sprintf((char*)xmlbuf,"/t/t<%s>%s</%s>\n\0",(char*)tag,(char*)data,(char*)tag);
        return (xmlbuf + wind_strlen((char*)tag) * 2 + 8 + wind_strlen((char*)data));
    }
    else
    { 
        for(k = 0;k < (*(char*)tag-'0');k ++) 
           *((char*)xmlbuf + k) = '\t';
        wind_sprintf((char*)xmlbuf + k,"<%s>%s</%s>\n\0",(char*)tag + 1,(char*)data,(char*)tag + 1);
        return (xmlbuf + k + wind_strlen((char*)tag) * 2 + 4 + wind_strlen((char*)data));
    }
}

/*д��չ(��Ҫ����ת��)������Ԫ�� */ 
xstring_s *xml_write_extag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data) 
{ 
    xstring_s xs;
    xstring_s xdata[512]; 
    int len = wind_strlen((char*)data);
    int k;
    xs.text = (char*)xdata; /*xmlbuf + wind_strlen(tag) + 2; */ 
    xs.len = len;
    wind_memcpy(xs.text,data,len); 
    xml_data_uncode(&xs);
    *(xs.text + xs.len) = '\0';
    if(*(char*)tag > '9')
    { 
        wind_sprintf((char*)xmlbuf,"/t/t<%s>%s</%s>\n\0",(char*)tag,xs.text,(char*)tag);
        return (xmlbuf + wind_strlen((char*)tag) * 2 + 8 + xs.len);
    }
    else
    { 
        for(k = 0;k < (*(char*)tag-'0');k ++) 
           *((char*)xmlbuf + k) = '\t';
        wind_sprintf((char*)xmlbuf + k,"<%s>%s</%s>\n\0",(char*)tag + 1,xs.text,(char*)tag + 1);
        return (xmlbuf + k + wind_strlen((char*)tag) * 2 + 4 + xs.len);
    }
}

/*дע�� */ 
xstring_s *xml_write_note(xstring_s *xmlbuf,xstring_s *note) 
{ 
    wind_sprintf((char*)xmlbuf,"<!--%s-->\n\0",(char*)note);
    return (xmlbuf + wind_strlen((char*)note) + 8);
}

/*д���⴮д���� */ 
xstring_s *xml_write_buf(xstring_s *buf,xstring_s *str) 
{
    wind_sprintf((char*)buf,"%s\0",(char*)str);
    return (buf + wind_strlen((char*)str));
}


