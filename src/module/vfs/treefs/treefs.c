#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "treefs.h"
#include "treefile.h"
#ifdef WIND_FS_SUPPORT

w_dlist_s treefslist;
WIND_POOL(treefspool,WIND_TREEFS_MAX_NUM,sizeof(w_treefs_s));
w_err_t _wind_treefs_mod_init(void)
{
    w_err_t err;
    w_treefs_s *tfs;
    DLIST_INIT(treefslist);
    err = wind_pool_create("treefs",treefspool,sizeof(treefspool),sizeof(w_treefs_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    tfs = wind_treefs_create("tfs0");
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_MEM);
    return err;
}

void *tfs_mem_malloc(w_int32_t size)
{
    void *ptr = wind_falloc(size,252);
    return ptr;
}

w_err_t tfs_mem_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}


w_err_t wind_treefs_format(w_treefs_s *tfs)
{
    char *buff;
    w_treefile_s *file;
    w_treefile_s *root = tfs->root;
    if(root != W_NULL)
        treefile_rm(root);
    root = treefs_mk_subnode(W_NULL,"",1);
    if(!root)
    {
        wind_error("make treefs root failed");
        return W_ERR_FAIL;
    }
        
#if 1
    tfs->root = root;
    treefile_create(tfs,"/var/");
    treefile_create(tfs,"/mnt/");
    treefile_create(tfs,"/usr/");
    treefile_create(tfs,"/sys/");
    treefile_create(tfs,"/proc/");
    treefile_create(tfs,"/bin/");
    treefile_create(tfs,"/usr/config.txt");
    treefile_create(tfs,"/usr/wind-os.log");
    treefile_create(tfs,"/usr/env.cfg");
    file = treefile_open(tfs,"/usr/env.cfg",TF_FMODE_W);
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


w_treefs_s *wind_treefs_get(const char *name)
{
	return (w_treefs_s*)wind_obj_get(name,&treefslist);
}

w_err_t wind_treefs_init(w_treefs_s *treefs,const char *name)
{
    wind_notice("init treefs:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(treefs != W_NULL,W_ERR_PTR_NULL);
    wind_obj_init(&treefs->obj,TREEFS_MAGIC,name,&treefslist);
    CLR_F_TREEFS_POOL(treefs);
    treefs->fs_size = 0;
    treefs->root = W_NULL;
    return W_ERR_OK;
}

w_treefs_s *wind_treefs_create(char *name)
{
    w_err_t err;
    w_treefs_s *treefs;
    treefs = wind_pool_malloc(&treefspool);
    WIND_ASSERT_RETURN(treefs != W_NULL,W_NULL);
    err = wind_treefs_init(treefs,name);
    if(err == W_ERR_OK)
    {
        SET_F_TREEFS_POOL(treefs);
        return treefs;
    }
    tfs_mem_free(treefs);
    return W_NULL;
}

w_err_t wind_treefs_destroy(w_treefs_s *treefs)
{
    w_err_t err;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(treefs != W_NULL,W_ERR_PTR_NULL);
    wind_notice("destroy treefs:%s",wind_obj_name(&treefs->obj));
    err = wind_obj_deinit(&treefs->obj,TREEFS_MAGIC,&treefslist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    if(IS_F_TREEFS_POOL(treefs))
        wind_pool_free(&treefspool,treefs);
    return W_ERR_OK;    
}


#endif //#ifdef WIND_FS_SUPPORT

