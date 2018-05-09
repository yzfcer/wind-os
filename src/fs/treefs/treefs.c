#include "wind_file.h"
typedef struct treefs_s
{
    tree_s treenode;
    char *filename;
    w_uint8_t isdir;
    fmode_e mode;
    w_uint8_t *data;
}treefs_node_s;

static void *treefs_malloc(w_int32_t size)
{
    return wind_malloc(size);
}
static w_err_t treefs_free(void *ptr)
{
    if(ptr == NULL)
        return ERR_OK;
    return wind_free(ptr);
}

static treefs_node_s *treefs_root = NULL;
static w_int32_t path_compare(const char *path,const char *name)
{
    int i,len;
    len = wind_strlen(name);
    if(path[len] != '/')
        return -1;
    for(i = 0;i < len;i ++)
    {
        if(path[i] != name[i])
            return -1;
    }
    return len;
}

//将路径分割成段
static int split_path(char *path,char **layers)
{
    w_int32_t idx,i,j;
    int len = wind_strlen(path);
    idx = path[0] == '/'?1:0;
    for(i = 0;i < TREEFS_DIR_LAYCNT;i ++)
    {
        layers[i] = &path[idx];
        for(j = idx;j < len;j ++)
        {
            if(path[j] == '/')
            {
                path[j] = 0;
                idx = j + 1;
                break;
            }
        }
        if(j >= len)
            break;
    }
    if(i >= TREEFS_DIR_LAYCNT)
        return -1;
    return i;
}

static treefs_node_s *search_node(treefs_node_s *parent,const char *path)
{
    w_int32_t len,cnt,i;
    w_bool_t flag;
    dnode_s *node;
    treefs_node_s *fsnode = NULL,*root = NULL;
    char **nameseg;
    char *pathname;
    nameseg = (char **)treefs_malloc(TREEFS_DIR_LAYCNT * sizeof(char*));
    len = wind_strlen(path);
    pathname = treefs_malloc(len+1);
    if(pathname == NULL || nameseg == NULL)
        goto SEARCH_COMPLETE;
    wind_strcpy(pathname,path);
    pathname[len] = 0;
    cnt = split_path(pathname,nameseg);
    if(cnt < 0)
        goto SEARCH_COMPLETE;
    root = treefs_root;
    for(i = 0;i < cnt;i ++)
    {
        flag = B_FALSE;
        foreach_node(node,root->treenode.child_list)
        {
            fsnode = DLIST_OBJ(node,treefs_node_s,treenode.brother_node);
            if(wind_strcmp(fsnode->filename,nameseg[i]) == 0)
            {
                root = fsnode;
                flag = B_TRUE;
                break;
            }
        }
        if(flag == B_FALSE)
            goto SEARCH_COMPLETE;
    }
SEARCH_COMPLETE:
    treefs_free(pathname);
    treefs_free(nameseg);
    return root;
}

static w_err_t mk_subnode(treefs_node_s *parent,char *name,w_uint8_t isdir)
{
    int len = wind_strlen(name);
    treefs_node_s *fsnode = treefs_malloc(sizeof(treefs_node_s));
    WIND_ASSERT_RETURN(fsnode != NULL,ERR_MEM);
    fsnode->filename = treefs_malloc(len+1);
    WIND_ASSERT_TODO(fsnode->filename != NULL,treefs_free(fsnode),ERR_MEM);
    fsnode->data = NULL;
    
    
}

static treefs_node_s *treefs_mk_file(treefs_node_s *parent,const char *name,w_uint8_t isdir)
{
    w_int32_t len,cnt,i;
    w_bool_t flag;
    dnode_s *node;
    treefs_node_s *fsnode = NULL,*root = NULL;
    char **nameseg;
    char *pathname;
    nameseg = (char **)treefs_malloc(TREEFS_DIR_LAYCNT * sizeof(char*));
    len = wind_strlen(path);
    pathname = treefs_malloc(len+1);
    if(pathname == NULL || nameseg == NULL)
        goto MKFILE_COMPLETE;
    wind_strcpy(pathname,path);
    pathname[len] = 0;
    cnt = split_path(pathname,nameseg);
    if(cnt < 0)
        goto MKFILE_COMPLETE;
    root = treefs_root;
    for(i = 0;i < cnt;i ++)
    {
        flag = B_FALSE;
        foreach_node(node,root->treenode.child_list)
        {
            fsnode = DLIST_OBJ(node,treefs_node_s,treenode.brother_node);
            if(wind_strcmp(fsnode->filename,nameseg[i]) == 0)
            {
                root = fsnode;
                flag = B_TRUE;
                break;
            }
        }
        if(flag == B_FALSE)
        {
            
        }
            goto MKFILE_COMPLETE;
    }
MKFILE_COMPLETE:
    treefs_free(pathname);
    treefs_free(nameseg);
    return root;
}

static w_err_t treefs_rm_file(treefs_node_s *parent)
{
    
}




file_s* treefs_open(const char *path,fmode_e fmode)
{
    treefs_node_s *node;
    treefs_node_s *root = treefs_root;
    node = search_node(root,path);
    if((node == NULL) && !(fmode & FMODE_CRT))
        return NULL;
    if(node == NULL)
    {
        treefs_
    }
    if(fmode & fmode)
    return NULL;
}

w_err_t treefs_close(file_s* file)
{
    return ERR_OK;
}
w_err_t treefs_seek(file_s* file,w_int32_t offset)
{
    return ERR_OK;
}
w_int32_t treefs_ftell(file_s* file)
{
    return ERR_OK;
}
w_int32_t treefs_read(file_s* file,char *buff, w_int32_t size)
{
    return ERR_OK;
}
w_int32_t treefs_write(file_s* file,char *buff, w_int32_t size)
{
    return ERR_OK;
}
w_err_t treefs_fgets(file_s* file,char *buff, w_int32_t maxlen)
{
    return ERR_OK;
}
w_err_t treefs_fputs(file_s* file,char *buff)
{
    return ERR_OK;
}

