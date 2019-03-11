#ifndef XML_H__
#define XML_H_

typedef struct __XSTRING XSTRING;
struct __XSTRING
{
    char *text;
    int len;
};

typedef struct __XNODE XNODE;
struct __XNODE
{
    XSTRING name;
    int index;
    XNODE *parentNode;
    int level;
    int tagCount;
    XSTRING data;
};

typedef struct __XTREE XTREE;
struct __XTREE
{
    
    int nodeCount;  
    int nodeMaxCount;
    XNODE *preNode;  
    XNODE *curNode;
    XSTRING *xBuf;
    XSTRING *xBufPos;
    XSTRING *xBufEnd;
    XNODE *nodeList;
    XNODE *rootNode;
};

#endif
