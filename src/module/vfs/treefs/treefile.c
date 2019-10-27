#include "treefs.h"
#include "treefile.h"
#include "wind_tree.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_TREEFS_SUPPORT

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


w_treefile_s *treefs_mk_subnode(w_treefile_s *parent,char *nodename,w_uint8_t isdir)
{
    w_treefile_s *treefile;
    int len = wind_strlen(nodename);
    WIND_ASSERT_RETURN(nodename != W_NULL,W_NULL);
    treefile = tfs_mem_malloc(sizeof(w_treefile_s));
    WIND_ASSERT_RETURN(treefile != W_NULL,W_NULL);
    treefile->filename = tfs_mem_malloc(len+1);
    WIND_ASSERT_TODO_RETURN(treefile->filename != W_NULL,tfs_mem_free(treefile),W_NULL);
    
    treefile->magic = TREEFILE_MAGIC;
    wind_tree_init(&treefile->tree);
    wind_memset(treefile->filename,0,len+1);
    wind_strcpy(treefile->filename,nodename);
    
    treefile->isdir = isdir?1:0;
    treefile->mode = 0;
    treefile->offset = 0;
    treefile->filelen = 0;
    treefile->bufflen = 0;
    DLIST_INIT(treefile->datalist);
    if(parent != W_NULL)
    {        
        wind_tree_insert_child(&parent->tree,&treefile->tree);
    }
    return treefile;
}



static w_treefile_s *treefs_get_childnode(w_tree_s *parent,char *childname)
{
    w_dnode_s *dnode;
    w_treefile_s *treefile;
    foreach_node(dnode,&parent->child_list)
    {
        treefile = NODE_TO_TREEFILE(dnode);
        if(wind_strcmp(treefile->filename,childname) == 0)
        {
            return treefile;
        }
    }
    return W_NULL;
}

static w_treefile_s *treefs_search_node(w_treefs_s *tfs,const char *path)
{
    w_err_t err;
    w_int32_t len,cnt,i;
    w_treefile_s *file = W_NULL;
    char **nameseg = W_NULL;
    char *pathname = W_NULL;
    wind_debug("search node path:%s",path);
    do
    {
        err = W_ERR_OK;
        nameseg = (char **)tfs_mem_malloc(TREEFS_DIR_LAYCNT * sizeof(char*));
        WIND_ASSERT_BREAK(nameseg != W_NULL,W_ERR_MEM,"malloc nameseg failed");
        len = wind_strlen(path);
        pathname = tfs_mem_malloc(len+1);
        WIND_ASSERT_BREAK(pathname != W_NULL,W_ERR_MEM,"malloc pathname failed");
        wind_memset(pathname,0,len+1);
        wind_strcpy(pathname,path);
        pathname[len] = 0;
        cnt = wind_strsplit(pathname,'/',nameseg,TREEFS_DIR_LAYCNT);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_INVALID,"split path failed");
        file = tfs->root;
        WIND_CHECK_BREAK(cnt != 1,W_ERR_OK);
        for(i = 1;i < cnt;i ++)
        {
            file = treefs_get_childnode(&file->tree,nameseg[i]);
            WIND_CHECK_BREAK(file != W_NULL,W_ERR_FAIL);
        }
        WIND_CHECK_BREAK(err == W_ERR_OK,err);
    }while(0);
        
    if(pathname)
        tfs_mem_free(pathname);
    if(nameseg)
        tfs_mem_free(nameseg);
    return file;
}

static w_treefile_s *treefs_make_node(w_treefs_s *tfs,const char *path)
{
    char *ptr;
    w_err_t err;
    w_int32_t len;
    w_uint8_t isdir = 0;
    char *nodename;
    w_treefile_s *treefile;
    char *dirname = W_NULL;
    len = wind_strlen(path);

    do
    {
        err = W_ERR_OK;
        dirname = tfs_mem_malloc(len+1);
        WIND_ASSERT_BREAK(dirname != W_NULL,W_ERR_MEM,"alloc dirname failed");
        wind_strcpy(dirname,path);
        if(dirname[len - 1] == '/')
        {
            dirname[len - 1] = 0;
            isdir = 1;
        }
        ptr = wind_strrchr(dirname,'/');
        WIND_ASSERT_BREAK(ptr != W_NULL,W_ERR_INVALID,"invalid path");
        *ptr = 0;
        nodename = &ptr[1];
        treefile = treefs_search_node(tfs,dirname);
        WIND_ASSERT_BREAK(treefile != W_NULL,W_ERR_INVALID,"search filenode failed");

        treefile = treefs_mk_subnode(treefile,nodename,isdir);
        WIND_ASSERT_BREAK(treefile != W_NULL,W_ERR_FAIL,"make filenode failed");
        treefile->tfs = tfs;
    }while(0);
        
    if(dirname != W_NULL)
        tfs_mem_free(dirname);
    return treefile;
}



w_treefile_s *treefile_create(w_treefs_s *tfs,const char *path)
{
    w_int32_t len;
    w_treefile_s *treefile = W_NULL;
    treefile = treefs_search_node(tfs,path);
    if(treefile != W_NULL)
    {
        wind_warn("file:%s has been existing.",path);
        return treefile;
    }
    treefile = treefs_make_node(tfs,path);
    len = wind_strlen(path);
    treefile->isdir = path[len-1] == '/'?1:0;
    return treefile;
}

w_err_t treefile_remove(w_treefile_s *file)
{
    w_dnode_s *dnode;
    w_tree_s *tree;
    w_treefile_s *subfile;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    tree = &file->tree;
    wind_debug("rm %s\r\n",file->filename);
    foreach_node(dnode,&tree->child_list)
    {
        subfile = NODE_TO_TREEFILE(dnode);
        treefile_remove(subfile);
    }
    if(file->tree.parent)
        wind_tree_remove_child(file->tree.parent,&file->tree);
    foreach_node(dnode,&file->datalist)
    {
        dlist_remove(&file->datalist,dnode);
        tfs_mem_free(dnode);
    }
    wind_debug("treefs free name;0x%x\r\n",file->filename);
    tfs_mem_free(file->filename);
    tfs_mem_free(file);
    return W_ERR_OK;
}


w_treefile_s* treefile_open(w_treefs_s *tfs,const char *path,w_uint16_t mode)
{
    w_bool_t is_crt;
    w_treefile_s *file;

    is_crt = (mode & TF_FMODE_CRT)?W_TRUE:W_FALSE;
    file = treefs_search_node(tfs,path);
    if((file == W_NULL) && (!is_crt))
    {
        return W_NULL;
    }
        
    if(file == W_NULL)
        file = treefs_make_node(tfs,path);
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

w_err_t treefile_close(w_treefile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode != 0,W_ERR_FAIL);
    file->mode = 0;
    file->offset = 0;
    return W_ERR_OK;
}

w_bool_t treefile_existing(w_treefs_s *tfs,const char *path)
{
    w_treefile_s *file;
    file = treefs_search_node(tfs,path);
    if(file != W_NULL)
        return W_TRUE;
    return W_FALSE;
}

w_err_t treefile_seek(w_treefile_s* file,w_int32_t offset)
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

w_int32_t treefile_ftell(w_treefile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(file->mode != 0,W_ERR_FAIL);
    return file->offset;
}

w_int32_t treefile_read(w_treefile_s* file,w_uint8_t *buff, w_int32_t size)
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


w_int32_t treefile_write(w_treefile_s* file,w_uint8_t *buff, w_int32_t size)
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
        dnode = tfs_mem_malloc(TREEFS_BLK_SIZE + sizeof(w_dnode_s));
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
        tfs_mem_free(dnode);
        file->bufflen -= TREEFS_BLK_SIZE;
    }
    return wsize;
}




w_err_t treefile_readdir(w_treefile_s* dir,w_treefile_s **sub)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_treefile_s *subfile;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->magic == TREEFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    subfile = *sub;
    do
    {
        err = W_ERR_OK;
        if(subfile == W_NULL)
        {
            dnode = dir->tree.child_list.head;
            WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
        }
        else
        {
            dnode = subfile->tree.treenode.next;
            WIND_CHECK_BREAK(dnode != W_NULL,W_ERR_NOFILE);
        }
        subfile = NODE_TO_TREEFILE(dnode);
        WIND_CHECK_BREAK(subfile->magic == TREEFILE_MAGIC,W_ERR_FAIL);
        WIND_ASSERT_BREAK(subfile->magic == TREEFILE_MAGIC,W_ERR_INVALID,"invalid treefile dound");
        
    }while(0);
    if(err == W_ERR_OK)
        *sub = subfile;
    else
        *sub = W_NULL;
    return err;
}


#endif

