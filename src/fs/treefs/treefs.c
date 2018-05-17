#include "wind_file.h"
#include "treefs.h"
#include "wind_tree.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_FS_SUPPORT
static treefile_s *treefile_rootnode = NULL;

static void *treefile_malloc(w_int32_t size)
{
    return wind_malloc(size);
}

static w_err_t treefile_free(void *ptr)
{
    if(ptr == NULL)
        return ERR_OK;
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

static dnode_s *get_node_by_offset(dlist_s *list,w_uint32_t offset)
{
    int idx;
    dnode_s *node;
    w_int32_t nodeidx = offset / TREEFS_BLK_SIZE;
    idx = 0;
    foreach_node(node,list)
    {
        if(node == NULL)
            return NULL;
        if(idx == nodeidx)
            return node;
        idx ++;
    }
    return NULL;
}

static w_uint32_t get_dataidx_by_offset(w_uint32_t offset)
{
    return offset % TREEFS_BLK_SIZE;
}


#if 0
static w_int32_t path_compare(const char *path,const char *nodename)
{
    int i,len;
    len = wind_strlen(nodename);
    if(path[len] != '/')
        return -1;
    for(i = 0;i < len;i ++)
    {
        if(path[i] != nodename[i])
            return -1;
    }
    return len;
}
#endif


//将路径分割成段
int split_path(char *path,char **layers,w_int32_t layercnt)
{
    w_int32_t i,j,cnt = 0;
    int len = wind_strlen(path)+1;
    j = 0;
    //wind_printf("path:%s\r\n",path);
    for(i = 0;i < layercnt;i ++)
    {
        if(cnt >= layercnt)
            return -1;
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
    //for(j = 0;j < cnt;j ++)
    //    wind_printf("node:%s\r\n",layers[j]);
    return cnt;
}

static treefile_s *mk_subnode(treefile_s *parent,char *nodename,w_uint8_t isdir)
{
    treefile_s *fsnode;
    attr_u *attr;
    int len = wind_strlen(nodename);
    WIND_ASSERT_RETURN(nodename != NULL,NULL);
    fsnode = treefile_malloc(sizeof(treefile_s));
    WIND_ASSERT_RETURN(fsnode != NULL,NULL);
    fsnode->magic = TREEFILE_MAGIC;
    fsnode->filename = treefile_malloc(len+1);
    WIND_ASSERT_TODO(fsnode->filename != NULL,treefile_free(fsnode),NULL);
    wind_tree_init(&fsnode->tree);
    wind_memset(fsnode->filename,0,len+1);
    wind_strcpy(fsnode->filename,nodename);
    
    fsnode->attr = 0755;
    attr = (attr_u*)&fsnode->attr;
    attr->sattr.isdir = isdir?1:0;
    fsnode->mode = 0;
    fsnode->offset = 0;
    fsnode->filelen = 0;
    fsnode->bufflen = 0;
    //fsnode->data = NULL;
    DLIST_INIT(fsnode->datalist);
    if(parent != NULL)
        wind_tree_insert_child(&parent->tree,&fsnode->tree);
    return fsnode;
}

static treefile_s *search_node(const char *path,w_bool_t create)
{
    w_int32_t len,cnt,i;
    w_uint8_t isdir,dir_f;
    w_bool_t flag;
    dnode_s *node;
    treefile_s *fsnode = NULL,*parent = NULL;
    char **nameseg;
    char *pathname;
    nameseg = (char **)treefile_malloc(TREEFS_DIR_LAYCNT * sizeof(char*));
    len = wind_strlen(path);
    isdir = path[len-1]=='/'?1:0;
    pathname = treefile_malloc(len+1);
    if(pathname == NULL || nameseg == NULL)
        goto SEARCH_COMPLETE;
    wind_strcpy(pathname,path);
    pathname[len] = 0;
    cnt = split_path(pathname,nameseg,TREEFS_DIR_LAYCNT);
    if(cnt < 0)
        goto SEARCH_COMPLETE;
    parent = treefs_get_root();
    if(cnt == 1)
        goto SEARCH_COMPLETE;
    for(i = 1;i < cnt;i ++)
    {
        flag = B_FALSE;
        foreach_node(node,&parent->tree.child_list)
        {
            fsnode = DLIST_OBJ(node,treefile_s,tree.treenode);
            if(wind_strcmp(fsnode->filename,nameseg[i]) == 0)
            {
                parent = fsnode;
                flag = B_TRUE;
                break;
            }
        }
        if(flag == B_FALSE)
        {
            if(create)
            {
                dir_f = (i == cnt - 1)?isdir:1;
                parent = mk_subnode(parent,nameseg[i],dir_f);
                if(parent == NULL)
                {
                    wind_warn("create fsnode failed.");
                    goto SEARCH_COMPLETE;
                }
            }
            else
            {
                parent = NULL;
                goto SEARCH_COMPLETE;
            }
        }
    }
SEARCH_COMPLETE:
    treefile_free(pathname);
    treefile_free(nameseg);
    return parent;
}


treefile_s *treefs_mk_file(const char *path)
{
    w_int32_t len;
    attr_u *uattr;
    treefile_s *fsnode = NULL;
    fsnode = search_node(path,B_TRUE);
    WIND_ASSERT_RETURN(fsnode != NULL,NULL);
    len = wind_strlen(path);
    uattr = (attr_u*)&fsnode->attr;
    uattr->sattr.isdir = path[len-1] == '/'?1:0;
    return fsnode;
}

w_err_t treefs_rm_file(treefile_s *file)
{
    dnode_s *node;
    tree_s *tree;
    treefile_s *subfile;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    tree = &file->tree;
    if(wind_strcmp(file->filename,"") == 0)
        return ERR_INVALID_PARAM;
    wind_printf("rm %s\r\n",file->filename);
    foreach_node(node,&tree->child_list)
    {
        subfile = DLIST_OBJ(node,treefile_s,tree.treenode);
        treefs_rm_file(subfile);
    }
    if(file->tree.parent)
        wind_tree_remove_child(file->tree.parent,&file->tree);
    treefile_free(file->filename);
    foreach_node(node,&file->datalist)
    {
        dlist_remove(&file->datalist,node);
        treefile_free(node);
    }
    treefile_free(file);
    return ERR_OK;
}

w_err_t treefs_format(void)
{
    char *buff;
    treefile_s *file;
    treefile_s *root = treefs_get_root();
    if(root != NULL)
        treefs_rm_file(root);
    else
        root = mk_subnode(NULL,"",1);
    if(!root)
        return ERR_FAIL;
    treefs_set_root(root);
    treefs_mk_file("/var/");
    treefs_mk_file("/mnt/");
    treefs_mk_file("/usr/");
    treefs_mk_file("/sys/");
    treefs_mk_file("/proc/");
    treefs_mk_file("/bin/");
    treefs_mk_file("/usr/config.txt");
    treefs_mk_file("/usr/wind-os.log");
    treefs_mk_file("/usr/env.cfg");
    file = treefile_open("/usr/env.cfg",FMODE_W);
    buff = "os=wind-os\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "version=1.1.23\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "path=/;/mnt/;/usr/;;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "usr=root;wind;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    buff = "passwd=wind;wind;\r\n";
    treefile_write(file,(w_uint8_t*)buff,wind_strlen(buff));
    treefile_close(file);
    return ERR_OK;
}

w_err_t treefile_get_attr(treefile_s *file,tf_attr_s *attr)
{
    attr_u uattr;
    uattr.vattr = file->attr;
    wind_memcpy(attr,&uattr.sattr,sizeof(attr_u));
    return ERR_OK;
}

w_err_t treefile_set_attr(treefile_s *file,tf_attr_s *attr)
{
    attr_u uattr;
    uattr.vattr = file->attr;
    wind_memcpy(&uattr.sattr,attr,sizeof(attr_u));
    return ERR_OK;
}


treefile_s* treefile_open(const char *path,w_uint16_t mode)
{
    w_bool_t is_crt;
    treefile_s *file;

    is_crt = (mode & FMODE_CRT)?B_TRUE:B_FALSE;
    file = search_node(path,is_crt);
    WIND_ASSERT_RETURN(file != NULL,NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,NULL);
    WIND_ASSERT_RETURN(file->mode == 0,NULL);
    if(file == NULL)
        return NULL;
    file->mode = (w_uint8_t)mode;
    if(mode & FMODE_A)
        file->offset = file->filelen;
    else
        file->offset = 0;
    return file;
}

w_err_t treefile_close(treefile_s* file)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(file->mode != 0,ERR_FAIL);
    file->mode = 0;
    return ERR_OK;
}

w_bool_t treefile_existing(const char *path)
{
    treefile_s *file;
    file = search_node(path,B_FALSE);
    if(file != NULL)
        return B_TRUE;
    return B_FALSE;
}

w_err_t treefile_seek(treefile_s* file,w_int32_t offset)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(file->mode != 0,ERR_FAIL);
    if(offset < file->filelen)
        file->offset = offset;
    else
        file->offset = file->filelen;
    return ERR_OK;
}

w_int32_t treefile_ftell(treefile_s* file)
{
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,ERR_INVALID_PARAM);
    WIND_ASSERT_RETURN(file->mode != 0,ERR_FAIL);
    return file->offset;
}

w_int32_t treefile_read(treefile_s* file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t rsize,len,si;
    w_uint32_t dataidx,bufidx;
    dnode_s *node;
    w_uint8_t *src;
    //tf_attr_s attr;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,ERR_INVALID_PARAM);
    //treefile_get_attr(file,&attr);
    //WIND_ASSERT_RETURN(attr.u_r != 0,ERR_FAIL);
    WIND_ASSERT_RETURN((file->attr & FMODE_R),ERR_FAIL);
    WIND_ASSERT_RETURN((file->mode & FMODE_R),ERR_FAIL);
    
    WIND_ASSERT_RETURN(buff != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > 0,ERR_INVALID_PARAM);
    
    rsize = file->filelen - file->offset > size?size:file->filelen - file->offset;
    node = get_node_by_offset(&file->datalist,file->offset);
    dataidx = get_dataidx_by_offset(file->offset);
    bufidx = 0;
    si = rsize;
        
    for(;node != NULL;node = dnode_next(node))
    {
        src = (w_uint8_t*)node;
        src += sizeof(dnode_s);
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
    dnode_s *node;
    w_uint8_t *dest;
    //tf_attr_s attr;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,ERR_INVALID_PARAM);
    //treefile_get_attr(file,&attr);
    //WIND_ASSERT_RETURN(attr.u_w != 0,ERR_FAIL);
    WIND_ASSERT_RETURN((file->mode & (FMODE_W | FMODE_A)),ERR_FAIL);
    WIND_ASSERT_RETURN(buff != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(size > 0,ERR_INVALID_PARAM);
    while(file->bufflen < file->filelen + size)
    {
        node = treefile_malloc(TREEFS_BLK_SIZE + sizeof(dnode_s));
        WIND_ASSERT_RETURN(node != NULL,-1);
        dlist_insert_tail(&file->datalist,node);
        file->bufflen += TREEFS_BLK_SIZE;
    }
    
    //wsize = file->filelen - file->offset > size?size:file->filelen - file->offset;
    wsize = size;
    node = get_node_by_offset(&file->datalist,file->offset);
    dataidx = get_dataidx_by_offset(file->offset);
    bufidx = 0;
    si = wsize;
    
    for(;node != NULL;node = dnode_next(node))
    {
        dest = (w_uint8_t*)node;
        dest += sizeof(dnode_s);
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
        node = dlist_remove_tail(&file->datalist);
        WIND_ASSERT_RETURN(node != NULL,-1);
        treefile_free(node);
        file->bufflen -= TREEFS_BLK_SIZE;
    }
    return wsize;
}

treefile_s *treefile_readdir(treefile_s* file,w_int32_t index)
{
    int idx;
    dnode_s *node;
    treefile_s *sub;
    attr_u *attr;
    WIND_ASSERT_RETURN(file != NULL,NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,NULL);
    attr = (attr_u*)&file->attr;
    WIND_ASSERT_RETURN(attr->sattr.isdir == 1,NULL);
    
    idx = 0;
    foreach_node(node,&file->tree.child_list)
    {
        sub = (treefile_s*)DLIST_OBJ(node,treefile_s,tree.treenode);
        if(idx == index)
            return sub;
        idx ++;
    }
    return NULL;
}


w_err_t treefile_fgets(treefile_s* file,char *buff, w_int32_t maxlen)
{
    return ERR_OK;
}
w_err_t treefile_fputs(treefile_s* file,char *buff)
{
    return ERR_OK;
}

#endif

