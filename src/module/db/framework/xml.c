#include < stdlib.h >  
#include < stdio.h >  
#include < string.h >  
//#include < time.h >  
//#include < varargs.h >  
//#include"xmlparse.h" 
#include"xml.h"
#include"wind_debug.h"
/*xml得到节点名值,返回:1节点标识名开头,2节点标识名结尾,3注释,4元素数据  <= 0有错误 */ 
#define XVAL_NBEGIN 1 
#define XVAL_NEND 2 
#define XVAL_NOTE 3 
#define XVAL_DATA 4 
#define XVAL_TAG 5  /*空标志 */ 
#define XVAL_NONE 0 
#define XVAL_ERROR -1 
#if 1

static int is_space(char c) /*是空否 */ 
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

static int is_name_tchar(int c) /*有效的名称前导符 */ 
{ 
    return (c  >= 'A' && c <= 'Z') || (c  >= 'a' && c <= 'z') || (c == '_'); 
}

int copy_xstring(xstring_s *dest_xs,xstring_s *src_xs) /*X字串赋值 */ 
{ 
    dest_xs->text = src_xs->text;
    dest_xs->len = src_xs->len;
    return 0;
}


int copy_xnode(xnode_s *dest_xnode,xnode_s *src_xnode) /*X节点赋值 */ 
{ 
    copy_xstring(&(dest_xnode->name),&(src_xnode->name));
    dest_xnode->index = src_xnode->index;
    dest_xnode->parentNode = src_xnode->parentNode;
    dest_xnode->level = src_xnode->level;
    dest_xnode->tagCount = src_xnode->tagCount;
    copy_xstring(&(dest_xnode->data),&(src_xnode->data)); 
    return 0;
}

/*增加同级节点: */ 
xnode_s *xnode_add_brother(xtree_s *xtree,xstring_s *name) 
{
    if(xtree->curNode->parentNode == W_NULL || xtree->nodeCount  >= xtree->nodeMaxCount)
        return W_NULL;
    (xtree->nodeCount) ++ ;
    xtree->preNode = xtree->curNode;
    xtree->curNode = &(xtree->nodeList[xtree->nodeCount-1]);
    xtree->curNode->index = xtree->nodeCount-1;
    copy_xstring(&(xtree->curNode->name),name);
    xtree->curNode->parentNode = xtree->preNode->parentNode;
    xtree->curNode->level = xtree->preNode->level;
    (xtree->curNode->parentNode->tagCount) ++ ;
    return (xtree->curNode);
}

/*增加子节点: */ 
xnode_s *xnode_add_child(xtree_s *xtree,xstring_s *name) 
{ 
    if(xtree->curNode == W_NULL || xtree->nodeCount  >= xtree->nodeMaxCount)
        return W_NULL;
    (xtree->nodeCount) ++ ;
    xtree->preNode = xtree->curNode;
    xtree->curNode = &(xtree->nodeList[xtree->nodeCount-1]);
    xtree->curNode->index = xtree->nodeCount-1;
    copy_xstring(&(xtree->curNode->name),name);
    xtree->curNode->parentNode = xtree->preNode;
    xtree->curNode->level = xtree->preNode->level + 1;
    (xtree->preNode->tagCount) ++ ;
    return (xtree->curNode);
}

/*返回父节点: */ 
xnode_s *xnode_get_parent(xtree_s *xtree) 
{ 
    if(xtree->curNode->parentNode == W_NULL)
        return W_NULL;
    xtree->preNode = xtree->curNode;
    xtree->curNode = xtree->curNode->parentNode;
    return (xtree->curNode);
}

/*修改当前节点元素值: */ 
int xnode_modify_data(xtree_s *xtree,xstring_s *data) 
{ 
    if(xtree->curNode == W_NULL)
        return -1;
    /*if(xtree->curNode->tagCount > 0)
        return -2; */ 
    copy_xstring(&(xtree->curNode->data),data); 
    return 0;
}

xstring_s *delete_space(xstring_s *xbuf) /*删除前导空 */ 
{ 
    xstring_s *p;
    for(p = xbuf;is_space(*(char*)p);p ++ ); /*去掉空字符 */ 
    return (p);
}

int xml_handle_attr(xtree_s *xtree,xstring_s *attr_str) /*处理属性:当作叶节点,子节点数为-1,返回增加的属性节点数 */ 
{ 
    xstring_s xn,xv;
    int k = 0;
    xstring_s *p1, *p, *p2;
    p = delete_space((xstring_s*)attr_str->text);
    while(p && *(char*)p)
    { 
        p1 = p;
        p2 = (xstring_s*)strchr((char*)p1,' = ');
        if(p2 == W_NULL)
            break;
        xn.text = (char*)p1;
        *(char*)p2 = '\0';
        xn.len = p2-p1;
        p = delete_space(p2 + 1);
        if( *(char*)p != '\"')
            break;
        p1 = p;
        p2 = (xstring_s*)strchr((char*)(p1 + 1),'"');
        if(p2 == W_NULL)
            break;
        xv.text = (char*)(p1 + 1);
        *(char*)p2 = '\0';
        xv.len = p2-p1-1;
        /*增加属性值节点 */ 
        if(xnode_add_child(xtree,&xn) == W_NULL)
            break;
        k ++ ;
        if(xnode_modify_data(xtree,&xv) < 0)
            break;
        xtree->curNode->tagCount = -1;
        xnode_get_parent(xtree);
        p = delete_space(p2 + 1);
    }
    return (k);
}

/*xml得到节点名值,返回:1节点标识名开头,2节点标识名结尾,3注释,4元素数据 0无 -1有错误 */ 
int xml_read_xnode_value(xtree_s *xtree,xstring_s *value_str) 
{ 
    xstring_s *p, *p1, *p2;
    int ivtype = XVAL_NONE;
    p = xtree->xBufPos;
    p = delete_space(xtree->xBufPos); /*删除前导空 */ 
    if( *(char*)p!= ' < ')
    {  /*数据 */ 
        p1 = p;
        p2 = (xstring_s*)strchr((char*)p1,' < ');
        value_str->text = (char*)p1;
        value_str->len = (p2?p2-p1:0);
        ivtype = (p2 && p2 < xtree->xBufEnd? XVAL_DATA:XVAL_NONE);
        xtree->xBufPos = (p2?p2:xtree->xBufEnd);
        goto OKExit33qqq;
    }

    /*前导为 < */ 
    p ++ ;
    if( *(char*)p == '/') 
    { /*标记尾 */ 
        p1 = p + 1;
        p2 = (xstring_s*)strchr((char*)p1,'>');
        if(p2 == W_NULL)
        { /*错误 */ 
            goto ErrExit33qqq;
        }
        else
        { 
            ivtype = XVAL_NEND;
            value_str->text = (char*)p1;
            value_str->len = p2-p1;
            xtree->xBufPos = p2 + 1;
        }
    }
    else if(is_name_tchar( *(char*)p)) 
    { /*标记头 */ 
        p1 = p;
        p2 = (xstring_s*)strchr((char*)p1,'>');
        if(p2 == W_NULL)
        { /*错误 */ 
            goto ErrExit33qqq;
        } 
        for(p = p1; *(char*)p!= ' ' && p < p2;p ++ );
        if( *((char*)(p2-1)) == '/')
        { /*处理空标记 < abc/ >  */ 
            ivtype = 11;
            value_str->text = (char*)p1;
            if(p  >= p2)
                p = p2-1;
            value_str->len = p-p1;
            if(xnode_add_child(xtree,value_str) == W_NULL)
                goto ErrExit33qqq;
            if(p < p2-1)/*增加属性节点 */ 
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
            if(p < p2) /*增加属性节点 */ 
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
        xtree->xBufPos = p2 + 1;
    }
    else if( *(char*)p == '!')
    {  
        if( *((char*)p + 1) == '-' &&  *((char*)p + 2) == '-') /*注释 */ 
        { 
            p += 3; 
            p1 = delete_space(p);
            p2 = (xstring_s*)strstr((char*)p1,"-->"); /*注释尾 */ 
            if(p2 == W_NULL)
            { /*错误 */ 
                goto ErrExit33qqq;
            }
            else
            { 
                ivtype = XVAL_NOTE;
                xtree->xBufPos = p2 + 3;
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
    xtree->xBufPos = xtree->xBufEnd;
    return (ivtype); 
}

/*读xml并解析到X树 */ 
int xml_parse(xstring_s *xmlbuf,int xmlsize,xtree_s *xtree) 
{ 
    xstring_s *p, *p1, *pend = xmlbuf + xmlsize;
    xstring_s xstr;
    xnode_s *xn;
    int k;
    int maxnn = 0,vtype = XVAL_NONE, vtype0 = XVAL_NONE;

    memset(xtree,0x00,sizeof(xtree_s));
    if(memcmp(xmlbuf,"< ?xmlversion = ",14)!= 0) 
    { 
        wind_error("无XML内容头!");
        return -1;
    }
    p = (xstring_s*)strstr((char*)xmlbuf,"? > ");
    if(p == W_NULL) 
    { 
        wind_error("XML内容头无结尾!");
        return -1;
    }
    p1 = delete_space(p + 2);
    for(p = p1,maxnn = 1;p < pend;p ++ )
    { 
        if( *(char*)p == '>' ||  *(char*)p == '"')
            maxnn ++ ;
        if( *(char*)p == '>')
            if( *((char*)p-1) == '/') 
            maxnn ++ ;/*空标记 */ 
    }
    maxnn = maxnn/2 + 2;
    xtree->xBuf = xmlbuf;
    xtree->xBufEnd = xmlbuf + xmlsize;
    xtree->xBufPos = p1;
    for(k = 0;k < 99;k ++ )
    { 
        vtype = xml_read_xnode_value(xtree,&xstr); 
        if(vtype == XVAL_NBEGIN)
            break;
        else if(vtype!= XVAL_NOTE)
            break;
    }

    xtree->nodeCount = 0;
    if (vtype!= XVAL_NBEGIN) 
    { 
        xtree->nodeMaxCount = 0;
        wind_debug("空XML内容");
        return 0;
    }
    wind_debug(" == = 申请节点存储空间 = %d",maxnn);
    xtree->nodeList = (xnode_s *)calloc(maxnn,sizeof(xnode_s)); /*申请节点存储空间 */ 
    if(xtree->nodeList == W_NULL) 
    { 
        wind_error("申请节点存储空间(%d)失败!",maxnn);
        return -2;
    }
    xtree->nodeMaxCount = maxnn;
    memset(xtree->nodeList,0x00,maxnn *sizeof(xnode_s));
    xtree->rootNode = &(xtree->nodeList[0]); /*根节点 */ 
    xtree->rootNode->parentNode = W_NULL;
    xtree->rootNode->index = xtree->rootNode->level = 0;
    *(xstr.text + xstr.len) = '\0';
    copy_xstring(&(xtree->rootNode->name),&xstr);
    xtree->curNode = xtree->rootNode;
    xtree->nodeCount = 1;
    k = 0;
    while(k < maxnn ) 
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
            xnode_get_parent(xtree); /*返回父节点: */ 
            break;
        case XVAL_DATA: 
            xnode_modify_data(xtree,&xstr);/*修改当前节点元素值: */ 
            break;
        default: 
            if(vtype > 10) 
                break;/*printf(" == = c增加%d/n", vtype-10); */ 
        }
    }
    *(char*)xmlbuf = '\0'; /*置已解析标志 */ 
    maxnn = xtree->curNode->index; /*最终当前节点号 */ 
    for(k = 0;k < xtree->nodeCount;k ++ )
    { 
        xn = &(xtree->nodeList[k]);
        if(xn->name.text) 
           *(xn->name.text + xn->name.len) = '\0';
        else 
            xn->name.text = (char*)xmlbuf;
        if(xn->data.text)
           *(xn->data.text + xn->data.len) = '\0';
        else 
            xn->data.text = (char*)xmlbuf; 
    }
    wind_debug ("节点数 = %d/n",xtree->nodeCount);
    if(maxnn)
        wind_error("XML文件不完整(%d)",maxnn);
    xtree->curNode = xtree->rootNode; 
    return (maxnn);
}

int xml_parse_free(xtree_s *xtree) 
{ 
    if(xtree->nodeMaxCount > 0) 
        free(xtree->nodeList);
    xtree->nodeMaxCount = 0;
    xtree->nodeCount = 0;
    return 0;
}

int print_xtree(xtree_s *xtree) 
{ 
    int k;
    xnode_s *xn;
    wind_debug(" ********xTree结构,总节点数 = %d,节点空间 = %d ********",xtree->nodeCount,xtree->nodeMaxCount);
    wind_debug("/t节点号父节点节点级子节数节点名节点数据");
    for(k = 0;k < xtree->nodeCount;k ++ )
    { 
        xn = &(xtree->nodeList[k]);
        wind_debug("/t%d%d%d%d[%s][%s]",xn->index,(xn->parentNode?xn->parentNode->index:-1), xn->level,xn->tagCount,xn->name.text,xn->data.text);
    }
    wind_debug(" *****************************************************************");
    return 0;
}

/*从当前节点往下取得节点: */ 
xnode_s *xml_get_xnode_by_name(xtree_s *xtree,xstring_s *nodename) 
{ 
    int k;
    xnode_s *xn;
    for(k = xtree->curNode->index + 1;k < xtree->nodeCount;k ++ ) 
    { 
        xn = &(xtree->nodeList[k]);
        if(memcmp(xn->name.text,nodename,xn->name.len) == 0)
            break;
    }
    if(k  >= xtree->nodeCount)
        return W_NULL;
    xtree->curNode = xn;
    return (xn);
}

/*从当前节点往下取得子节点(根据父节点名和节点名): */ 
xnode_s *xml_get_xnode(xtree_s *xtree,xstring_s *parent_name,xstring_s *nodename) 
{ 
     int k;
    xnode_s *xn;
    for(k = xtree->curNode->index + 1;k < xtree->nodeCount;k ++ ) 
    { 
        xn = &(xtree->nodeList[k]);
        if(memcmp(xn->name.text,nodename,xn->name.len) == 0 && memcmp(xn->parentNode->name.text,parent_name,xn->parentNode->name.len) == 0)
            break;
    }
    if(k  >= xtree->nodeCount)
        return W_NULL;
    xtree->curNode = xn;
    return (xtree->curNode);
}

/*从当前节点往下取得子节点(根据父节点和节点名): */ 
xnode_s *xml_get_child_xnode(xtree_s *xtree,xnode_s *parent_node, xstring_s *nodename) 
{ 
     int k;
    xnode_s *xn;
    for(k = xtree->curNode->index + 1;k < xtree->nodeCount;k ++ ) 
    { 
        xn = &(xtree->nodeList[k]);
        if(xn->level <= parent_node->level)
        return W_NULL;
        if(xn->parentNode == parent_node && memcmp(xn->name.text,nodename,xn->name.len) == 0)
            break;
    }
    if(k  >= xtree->nodeCount)
        return W_NULL;
    xtree->curNode = xn;
    return (xtree->curNode);
}

/*取得叶节点: */ 
xnode_s *xml_get_tag(xtree_s *xtree, xnode_s *parent_node,xstring_s *tagname) 
{ 
     int k;
    xnode_s *xn;
    if(parent_node == W_NULL) 
        return W_NULL;
    for(k = parent_node->index + 1;k < xtree->nodeCount;k ++ ) 
    { 
        xn = &(xtree->nodeList[k]);
        if(xn->level <= parent_node->level)
        return W_NULL;
        if(xn->parentNode == parent_node && memcmp(xn->name.text,tagname,xn->name.len) == 0)
            break;
    }
    if(k  >= xtree->nodeCount)
        return W_NULL;
    xtree->curNode = xn;
    return (xtree->curNode);
}

/*代码编码转换1: */ 
int xml_encode(xstring_s *xs,xstring_s *scode,xstring_s *dcode) 
{ 
    int k,lens = strlen((char*)scode),lend = strlen((char*)dcode),nrr = 0;
    xstring_s *tmpbuf, *p, *p0, *pt, *pend;
    if(strstr(xs->text,(char*)scode) == W_NULL) 
        return 0;
    pend = (xstring_s *)(xs->text + xs->len);
    k = lend-lens;
    if(k < 0)
        k = 0;
    tmpbuf = (xstring_s *)malloc( xs->len + k *100 + 1 );
    if(tmpbuf == W_NULL)
        return -1;
    memset(tmpbuf,0x00,xs->len + k *100);
    for(p0 = p = (xstring_s*)xs->text,pt = tmpbuf;p && nrr < 100;nrr ++ )
    { 
        p = (xstring_s*)strstr((char*)p0,(char*)scode);
        if(p == W_NULL || p > pend)
        { 
            memcpy(pt,p0,pend-p0);
            pt += pend-p0;
            break;
        }
        memcpy(pt,p0,p-p0);pt += p-p0;
        memcpy(pt,dcode,lend);pt += lend;
        p0 = p + lens;
    }
    pt = '\0';
    lens = strlen((char*)tmpbuf);
    memcpy(xs->text,tmpbuf,lens );
    xs->len = lens;
    *(xs->text + lens) = '\0';
    free(tmpbuf);
    return (nrr);
}

/*XML代码符号转换: */ 
int xml_data_encode(xstring_s *xs) 
{ 
    xml_encode(xs,(xstring_s*)"<",(xstring_s*)"<");
    xml_encode(xs,(xstring_s*)">",(xstring_s*)">");
    xml_encode(xs,(xstring_s*)"/\"",(xstring_s*)"\"");
    xml_encode(xs,(xstring_s*)"'",(xstring_s*)"'");
    xml_encode(xs,(xstring_s*)"&",(xstring_s*)"&");
    /*为防止包中分隔符与业务系统冲突,非标准转换 */ 
    xml_encode(xs,(xstring_s*)"|",(xstring_s*)":");
    return (0);
}

/*XML代码符号反转换: */ 
int xml_data_uncode(xstring_s *xs) 
{ 
    xml_encode(xs,(xstring_s*)"&",(xstring_s*)"&");
    xml_encode(xs,(xstring_s*)"<",(xstring_s*)"<");
    xml_encode(xs,(xstring_s*)">",(xstring_s*)">");
    xml_encode(xs,(xstring_s*)"/\"",(xstring_s*)"\"");
    xml_encode(xs,(xstring_s*)"'",(xstring_s*)"'");
    return (0);
}

/*写xml头 */ 
xstring_s *xml_write_head(xstring_s *xmlbuf,xstring_s *encode) 
{ 
    sprintf((char*)xmlbuf,"< ?xmlversion = \"1.0\"encoding = \"%s\"? > \n\0",(char*)encode);
    return (xmlbuf + strlen((char*)xmlbuf));
}

/*写标识头 */ 
xstring_s *xml_write_xnode_beg(xstring_s *xmlbuf,xstring_s *tag) 
{ 
    int k;
    if( *(char*)tag > '9'){ 
        sprintf((char*)xmlbuf,"<%s>\n\0",(char*)tag);
        return (xmlbuf + strlen((char*)tag) + 3);
    }
    else
    { 
        for(k = 0;k < ( *(char*)tag-'0');k ++ ) *((char*)xmlbuf + k) = '/t';
        sprintf((char*)xmlbuf + k,"<%s>\n\0",tag + 1);
        return (xmlbuf + k + strlen((char*)tag) + 2);
    }
}

/*写标识尾 */ 
xstring_s *xml_write_xnode_end(xstring_s *xmlbuf,xstring_s *tag) 
{ 
    int k;
    if( *(char*)tag > '9')
    { 
        sprintf((char*)xmlbuf,"</%s>\n\0",(char*)tag);
        return (xmlbuf + strlen((char*)tag) + 4);
    }
    else
    { 
        for(k = 0;k < ( *(char*)tag-'0');k ++ ) 
           *((char*)xmlbuf + k) = '/t';
        sprintf((char*)xmlbuf + k,"</%s>\n\0",(char*)tag + 1);
        return (xmlbuf + k + strlen((char*)tag) + 3);
    } 
}

/*写数据元素 */ 
xstring_s *xml_write_tag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data) 
{ 
    int k;
    if( *(char*)tag > '9')
    { 
        sprintf((char*)xmlbuf,"/t/t<%s>%s</%s>\n\0",(char*)tag,(char*)data,(char*)tag);
        return (xmlbuf + strlen((char*)tag) *2 + 8 + strlen((char*)data) );
    }
    else
    { 
        for(k = 0;k < ( *(char*)tag-'0');k ++ ) 
           *((char*)xmlbuf + k) = '/t';
        sprintf((char*)xmlbuf + k,"<%s>%s</%s>\n\0",(char*)tag + 1,(char*)data,(char*)tag + 1);
        return (xmlbuf + k + strlen((char*)tag) *2 + 4 + strlen((char*)data));
    }
}

/*写扩展(需要符号转换)的数据元素 */ 
xstring_s *xml_write_extag(xstring_s *xmlbuf,xstring_s *tag,xstring_s *data) 
{ 
    xstring_s xs;
    xstring_s xdata[512]; 
    int len = strlen((char*)data);
    int k;
    xs.text = (char*)xdata; /*xmlbuf + strlen(tag) + 2; */ 
    xs.len = len;
    memcpy(xs.text,data,len); 
    xml_data_uncode(&xs);
    *(xs.text + xs.len) = '\0';
    if( *(char*)tag > '9')
    { 
        sprintf((char*)xmlbuf,"/t/t<%s>%s</%s>\n\0",(char*)tag,xs.text,(char*)tag);
        return (xmlbuf + strlen((char*)tag) *2 + 8 + xs.len );
    }
    else
    { 
        for(k = 0;k < ( *(char*)tag-'0');k ++ ) 
           *((char*)xmlbuf + k) = '/t';
        sprintf((char*)xmlbuf + k,"<%s>%s</%s>\n\0",(char*)tag + 1,xs.text,(char*)tag + 1);
        return (xmlbuf + k + strlen((char*)tag) *2 + 4 + xs.len );
    }
}

/*写注释 */ 
xstring_s *xml_write_note(xstring_s *xmlbuf,xstring_s *note) 
{ 
    sprintf((char*)xmlbuf,"<!--%s-->\n\0",(char*)note);
    return (xmlbuf + strlen((char*)note) + 8);
}

/*写任意串写缓冲 */ 
xstring_s *xml_write_buf(xstring_s *buf,xstring_s *str) 
{ 
    sprintf((char*)buf,"%s\0",(char*)str);
    return (buf + strlen((char*)str));
}

#endif

