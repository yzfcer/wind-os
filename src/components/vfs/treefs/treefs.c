#include "treefs.h"
#include "wind_tree.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_FS_SUPPORT
#define NODE_TO_TREEFILE(node) (treefile_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((treefile_s*)0)->tree.treenode)))
static treefile_s *treefile_rootnode = W_NULL;

void *treefs_malloc(w_int32_t size)
{
    void *ptr = wind_malloc(size);
    return ptr;
}

w_err_t treefs_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}

static treefile_s *treefs_get_root(void)
{
    return treefile_rootnode;
}

static void treefs_set_root(treefile_s *root)
{
    treefile_rootnode = root;
}

static w_dnode_s *get_node_by_offset(w_dlist_s *list,w_uint32_t offset)
{
    int idx;
    w_dnode_s *dnode;
    w_int32_t nodeidx = offset / TREEFS_BLK_SIZE;
    idx = 0;
    foreach_node(dnode,list)
    {
        WIND_ASSERT_RETURN(dnode != W_NULL,W_NULL);
        if(idx == nodeidx)
            return dnode;
        idx ++;
    }
    return W_NULL;
}

static w_uint32_t get_dataidx_by_offset(w_uint32_t offset)
{
    return offset % TREEFS_BLK_SIZE;
}


//将路径分割成段
int split_path(char *path,char **layers,w_int32_t layercnt)
{
    w_int32_t i,j,cnt = 0;
    int len = wind_strlen(path)+1;
    j = 0;
    for(i = 0;i < layercnt;i ++)
    {
        if(cnt >= layercnt)
        {
            wind_error("path is too deep");
            return -1;
        }
        layers[i] = &path[j];
        for(;j < len;j ++)
        {
            if((path[j] == '/') || (path[j] == '\0'))
            {
                cnt ++;
                path[j] = 0;
                j ++;
                break;
            }
        }
        if(j >= len)
            break;
    }
    cnt = (layers[cnt-1][0] != 0?cnt:cnt -1);
    return cnt;
}

static treefile_s *mk_subnode(treefile_s *parent,char *nodename,w_uint8_t isdir)
{
    treefile_s *fsnode;
    int len = wind_strlen(nodename);
    WIND_ASSERT_RETURN(nodename != W_NULL,W_NULL);
    fsnode = treefs_malloc(sizeof(treefile_s));
    WIND_ASSERT_RETURN(fsnode != W_NULL,W_NULL);
    fsnode->filename = treefs_malloc(len+1);
    WIND_ASSERT_TODO_RETURN(fsnode->filename != W_NULL,treefs_free(fsnode),W_NULL);
    
    fsnode->magic = TREEFILE_MAGIC;
    wind_tree_init(&fsnode->tree);
    wind_memset(fsnode->filename,0,len+1);
    wind_strcpy(fsnode->filename,nodename);
    
    fsnode->isdir = isdir?1:0;
    fsnode->mode = 0;
    fsnode->offset = 0;
    fsnode->filelen = 0;
    fsnode->bufflen = 0;
    DLIST_INIT(fsnode->datalist);
    if(parent != W_NULL)
    {        
        wind_tree_insert_child(&parent->tree,&fsnode->tree);
    }
    return fsnode;
}



static treefile_s *get_childnode(w_tree_s *parent,char *childname)
{
    w_dnode_s *dnode;
    treefile_s *fsnode;
    foreach_node(dnode,&parent->child_list)
    {
        fsnode = NODE_TO_TREEFILE(dnode);
        if(wind_strcmp(fsnode->filename,childname) == 0)
        {
            return fsnode;
        }
    }
    return W_NULL;
}

static treefile_s *search_node(const char *path)
{
    w_int32_t len,cnt,i;
    treefile_s *file = W_NULL;
    char **nameseg;
    char *pathname;
    wind_debug("search node path:%s",path);
    nameseg = (char **)treefs_malloc(TREEFS_DIR_LAYCNT * sizeof(char*));
    len = wind_strlen(path);
    pathname = treefs_malloc(len+1);
    if(pathname == W_NULL || nameseg == W_NULL)
    {
        wind_error("alloc memory error");
        goto SEARCH_COMPLETE;
    }
        
    wind_memset(pathname,0,len+1);
    wind_strcpy(pathname,path);
    pathname[len] = 0;
    cnt = split_path(pathname,nameseg,TREEFS_DIR_LAYCNT);
    if(cnt < 0)
    {
        wind_error("split path failed");
        goto SEARCH_COMPLETE;
    }
        
    file = treefs_get_root();
    if(cnt == 1)
    {
        goto SEARCH_COMPLETE;
    }
        
    for(i = 1;i < cnt;i ++)
    {
        file = get_childnode(&file->tree,nameseg[i]);
        if(file == W_NULL)
        {
            wind_debug("get child node failed");
            goto SEARCH_COMPLETE;
        }
    }
SEARCH_COMPLETE:
    if(pathname)
        treefs_free(pathname);
    if(nameseg)
        treefs_free(nameseg);
    return file;
}

static treefile_s *make_node(const char *path)
{
    char *ptr;
    w_int32_t len;
    w_uint8_t isdir = 0;
    char *dirname;
    char *nodename;
    treefile_s *file;
    len = wind_strlen(path);
    dirname = treefs_malloc(len+1);
    WIND_ASSERT_RETURN(dirname != W_NULL,W_NULL);
    wind_strcpy(dirname,path);
    if(dirname[len - 1] == '/')
    {
        dirname[len - 1] = 0;
        isdir = 1;
    }
        
    ptr = wind_strrchr(dirname,'/');
    if(ptr == W_NULL)
    {
        wind_error("get dir failed,path:%s",path);
        treefs_free(dirname);
        return W_NULL;
    }
    *ptr = 0;
    nodename = &ptr[1];
    file = search_node(dirname);
    if(file == W_NULL)
    {
        wind_error("search filenode failed");
        treefs_free(dirname);
        return W_NULL;
    }
    file = mk_subnode(file,nodename,isdir);
    treefs_free(dirname);
    return file;
}



treefile_s *treefile_create(const char *path)
{
    w_int32_t len;
    treefile_s *fsnode = W_NULL;
    fsnode = search_node(path);
    if(fsnode != W_NULL)
    {
        wind_warn("file:%s has been existing.",path);
        return fsnode;
    }
    fsnode = make_node(path);
    len = wind_strlen(path);
    fsnode->isdir = path[len-1] == '/'?1:0;
    return fsnode;
}

w_err_t treefile_rm(treefile_s *file)
{
    w_dnode_s *dnode;
    w_tree_s *tree;
    treefile_s *subfile;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    tree = &file->tree;
    wind_printf("rm %s\r\n",file->filename);
    foreach_node(dnode,&tree->child_list)
    {
        subfile = NODE_TO_TREEFILE(dnode);
        treefile_rm(subfile);
    }
    if(file->tree.parent)
        wind_tree_remove_child(file->tree.parent,&file->tree);
    foreach_node(dnode,&file->datalist)
    {
        dlist_remove(&file->datalist,dnode);
        treefs_free(dnode);
    }
    wind_printf("treefs free name;0x%x",file->filename);
    treefs_free(file->filename);
    treefs_free(file);
    return W_ERR_OK;
}

w_err_t treefs_format(void)
{
    char *buff;
    treefile_s *file;
    treefile_s *root = treefs_get_root();
    if(root != W_NULL)
        treefile_rm(root);
    root = mk_subnode(W_NULL,"",1);
    if(!root)
    {
        wind_error("make treefs root failed");
        return W_ERR_FAIL;
    }
        
#if 1
    treefs_set_root(root);
    treefile_create("/var/");
    treefile_create("/mnt/");
    treefile_create("/usr/");
    treefile_create("/sys/");
    treefile_create("/proc/");
    treefile_create("/bin/");
    treefile_create("/usr/config.txt");
    treefile_create("/usr/wind-os.log");
    treefile_create("/usr/env.cfg");
    file = treefile_open("/usr/env.cfg",TF_FMODE_W);
    buff = "os=wind-os\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "version=1.1.23\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "path=/;/mnt/;/usr/;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "usr=root;wind;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "passwd=wind;wind;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    treefile_close(file);
    #endif
    return W_ERR_OK;
}


treefile_s* treefile_open(const char *path,w_uint16_t mode)
{
    w_bool_t is_crt;
    treefile_s *file;

    is_crt = (mode & TF_FMODE_CRT)?W_TRUE:W_FALSE;
    file = search_node(path);
    if((file == W_NULL) && (!is_crt))
    {
        return W_NULL;
    }
        
    if(file == W_NULL)
        file = make_node(path);
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(file->mode == 0,W_NULL);
    file->mode = (w_uint8_t)mode;
    if(mode & TF_FMODE_A)
        file->offset = file->filelen;
    else
        file->offset = 0;
    return file;
}

w_err_t treefile_close(treefile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode != 0,W_ERR_FAIL);
    file->mode = 0;
    return W_ERR_OK;
}

w_bool_t treefile_existing(const char *path)
{
    treefile_s *file;
    file = search_node(path);
    if(file != W_NULL)
        return W_TRUE;
    return W_FALSE;
}

w_err_t treefile_seek(treefile_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode != 0,W_ERR_FAIL);
    if(offset < file->filelen)
        file->offset = offset;
    else
        file->offset = file->filelen;
    return W_ERR_OK;
}

w_int32_t treefile_ftell(treefile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode != 0,W_ERR_FAIL);
    return file->offset;
}

w_int32_t treefile_read(treefile_s* file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t rsize,len,si;
    w_uint32_t dataidx,bufidx;
    w_dnode_s *dnode;
    w_uint8_t *src;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->isdir == 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN((file->mode & TF_FMODE_R),W_ERR_FAIL);
    
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(size > 0,W_ERR_INVALID);
    
    rsize = file->filelen - file->offset > size?size:file->filelen - file->offset;
    dnode = get_node_by_offset(&file->datalist,file->offset);
    dataidx = get_dataidx_by_offset(file->offset);
    bufidx = 0;
    si = rsize;
        
    for(;dnode != W_NULL;dnode = dnode_next(dnode))
    {
        src = (w_uint8_t*)dnode;
        src += sizeof(w_dnode_s);
        src += dataidx;
        len = TREEFS_BLK_SIZE - dataidx;
        len = si > len?len:si;
        wind_memcpy(&buff[bufidx],src,len);
        bufidx += len;
        si -= len;
        dataidx = 0;
        if(si <= 0)
            break;
    }
    file->offset += rsize;
    return rsize;
}

w_int32_t treefile_write(treefile_s* file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t wsize,len,si;
    w_uint32_t dataidx,bufidx;
    w_dnode_s *dnode;
    w_uint8_t *dest;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN((file->mode & (TF_FMODE_W | TF_FMODE_A)),W_ERR_FAIL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(size > 0,W_ERR_INVALID);
    while(file->bufflen < file->filelen + size)
    {
        dnode = treefs_malloc(TREEFS_BLK_SIZE + sizeof(w_dnode_s));
        WIND_ASSERT_RETURN(dnode != W_NULL,-1);
        dlist_insert_tail(&file->datalist,dnode);
        file->bufflen += TREEFS_BLK_SIZE;
    }
    
    wsize = size;
    dnode = get_node_by_offset(&file->datalist,file->offset);
    dataidx = get_dataidx_by_offset(file->offset);
    bufidx = 0;
    si = wsize;
    
    for(;dnode != W_NULL;dnode = dnode_next(dnode))
    {
        dest = (w_uint8_t*)dnode;
        dest += sizeof(w_dnode_s);
        dest += dataidx;
        len = TREEFS_BLK_SIZE - dataidx;
        len = si > len?len:si;
        wind_memcpy(dest,&buff[bufidx],len);
        bufidx += len;
        si -= len;
        dataidx = 0;
        if(si <= 0)
            break;
    }
    file->offset += wsize;
    file->filelen =  file->offset;
    while(file->bufflen >= file->filelen + TREEFS_BLK_SIZE)
    {
        dnode = dlist_remove_tail(&file->datalist);
        WIND_ASSERT_RETURN(dnode != W_NULL,-1);
        treefs_free(dnode);
        file->bufflen -= TREEFS_BLK_SIZE;
    }
    return wsize;
}

treefile_s *treefile_readdir(treefile_s* file,w_int32_t index)
{
    int idx;
    w_dnode_s *dnode;
    treefile_s *sub;
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_NULL);
    WIND_ASSERT_RETURN(file->isdir == 1,W_NULL);
    
    idx = 0;
    foreach_node(dnode,&file->tree.child_list)
    {
        sub = NODE_TO_TREEFILE(dnode);
        if(idx == index)
            return sub;
        idx ++;
    }
    return W_NULL;
}


w_err_t treefile_fgets(treefile_s* file,char *buff, w_int32_t maxlen)
{
    return W_ERR_OK;
}
w_err_t treefile_fputs(treefile_s* file,char *buff)
{
    return W_ERR_OK;
}

#endif

