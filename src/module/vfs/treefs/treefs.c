/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : treefs.c
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Treefs file system infomation operation function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.04.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.04.05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "treefs.h"
#include "treefile.h"
#if WIND_TREEFS_SUPPORT

w_dlist_s treefslist;
WIND_POOL(treefspool,WIND_TREEFS_MAX_NUM,sizeof(w_treefs_s));
w_err_t _wind_treefs_mod_init(void)
{
    w_err_t err;

    DLIST_INIT(treefslist);
    err = wind_pool_create("treefs",treefspool,sizeof(treefspool),sizeof(w_treefs_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

void *tfs_mem_malloc(w_int32_t size)
{
    void *ptr = wind_alloc(size,HP_ALLOCID_TREEFS);
    return ptr;
}

w_err_t tfs_mem_free(void *ptr)
{
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}

static void treefs_create_files(w_treefs_s *tfs)
{
    char *buff;
    w_treefile_s *file;
    static w_bool_t crt_flag = W_FALSE;
    if(crt_flag)
        return;
    crt_flag = W_TRUE;
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
}

w_err_t wind_treefs_format(w_treefs_s *tfs)
{
    w_treefile_s *root = tfs->root;
    if(root != W_NULL)
        treefile_remove(root);
    root = treefs_mk_subnode(W_NULL,"",1);
    if(!root)
    {
        wind_error("make treefs root failed");
        return W_ERR_FAIL;
    }
    tfs->root = root;
    treefs_create_files(tfs);
    return W_ERR_OK;
}


w_treefs_s *wind_treefs_get(const char *name)
{
	return (w_treefs_s*)wind_obj_get(name,&treefslist);
}

w_err_t wind_treefs_init(w_treefs_s *treefs,const char *name)
{
    w_int32_t len;
    char *objname = W_NULL;
    wind_notice("init treefs:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(treefs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);

    len = wind_strlen(name) + 1;
    objname = tfs_mem_malloc(len);
    WIND_ASSERT_RETURN(objname != W_NULL,W_ERR_MEM);
    wind_strcpy(objname,name);
    wind_obj_init(&treefs->obj,TREEFS_MAGIC,objname,&treefslist);
    CLR_F_TREEFS_POOL(treefs);
    treefs->fs_size = 0;
    treefs->root = W_NULL;       
    
    return W_ERR_OK;
}

w_treefs_s *wind_treefs_create(char *name)
{
    w_err_t err;
    w_treefs_s *treefs;
    treefs = (w_treefs_s *)wind_pool_malloc(&treefspool);
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
    
    WIND_ASSERT_RETURN(treefs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(treefs->obj.name != W_NULL,W_ERR_PTR_NULL);
    wind_notice("destroy treefs:%s",wind_obj_name(&treefs->obj));
    err = wind_obj_deinit(&treefs->obj,TREEFS_MAGIC,&treefslist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    err = treefile_remove(treefs->root);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    tfs_mem_free(treefs->obj.name);
    treefs->obj.name = W_NULL;
    if(IS_F_TREEFS_POOL(treefs))
        wind_pool_free(&treefspool,treefs);
    return W_ERR_OK;
}


#endif //#ifdef WIND_MODULE_VFS_SUPPORT

