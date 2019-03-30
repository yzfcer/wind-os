/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? wind_fs.c
**�?  �?  �? Jason Zhou
**最后修改日�? 
**�?       �? 文件系统标准化API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建�? 
** �? �? v1.0
** 日　�? 
** 描　�? 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改�? 
** 日　�? 
** 描　�? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_fs.h"
#include "wind_file.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_core.h"
#include "wind_dlist.h"
#include "wind_board_port.h"

#if WIND_FS_SUPPORT
#define NODE_TO_FS(dnode) (w_fs_s*)(((w_uint8_t*)(dnode))-((w_uint32_t)&(((w_fs_s*)0)->fsnode)))
static w_dlist_s fslist;

static w_err_t mount_param_check(char *fsname,char *blkname,char *path)
{
    w_fs_s *fs;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_REPEAT);
    if(wind_strlen(path) >= FS_MOUNT_PATH_LEN)
    {
        wind_error("mount path is too long");
        return W_ERR_INVALID;
    }
        
    if(wind_strlen(fsname) >= WFS_NAME_LEN)
    {
        wind_error("fs name is too long");
        return W_ERR_INVALID;
    }
    foreach_node(dnode,&fslist)
    {
        fs = NODE_TO_FS(dnode);
        if((fs->mount_path != W_NULL) && 
            (wind_strcmp(path,fs->mount_path) == 0))
        {
            wind_error("mount path has been used");
            return W_ERR_REPEAT;
        }
            
        if(blkname == W_NULL)
            continue;
        if((fs->blkdev != W_NULL) && 
            (fs->blkdev->obj.name != W_NULL) && 
            (wind_strcmp(blkname,fs->blkdev->obj.name) == 0))
        {
            wind_error("block device has been used");
            return W_ERR_REPEAT;
        }
            
    }
    return W_ERR_OK;
}

w_err_t wind_fs_regster(w_fs_s *fs,w_int32_t count)
{
    w_int32_t i;
    w_fs_s *fsi;    
    w_err_t err;
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(count > 0,W_ERR_INVALID);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(fs[i].magic == WIND_FS_MAGIC,W_ERR_INVALID);
        wind_notice("register fs:%s",fs[i].name);
        fsi = wind_fs_get(fs[i].name);
        if(fsi != W_NULL)
        {
            wind_notice("fs %s has been registered.\r\n",fsi->name);
            continue;
        }
        if(fs[i].ops->init)
        {
            err = fs[i].ops->init(&fs[i]);
            if(err != W_ERR_OK)
            {
                wind_error("fs:%s init failed:%d.",fs[i].name,err);
                continue;
            }
        }
        wind_disable_switch();
        dlist_insert_tail(&fslist,&fs[i].fsnode);
        wind_enable_switch();    
    }
    return W_ERR_OK;
}
w_err_t wind_fs_unregster(w_fs_s *fs)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fs->magic == WIND_FS_MAGIC,W_ERR_INVALID);
    wind_notice("unregister fs:%s",fs->name);
    wind_disable_switch();
    dnode = dlist_remove(&fslist,&fs->fsnode);
    wind_enable_switch();
    if(dnode == W_NULL)
    {
        wind_error("fs has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}

static w_err_t wind_all_fs_regster(void)
{
    extern w_fs_s fs_treefs[1];
    wind_fs_regster(fs_treefs,1);
    return W_ERR_OK;
}

w_err_t _wind_fs_mod_init(void)
{
    DLIST_INIT(fslist);
    _wind_file_mod_init();
    wind_all_fs_regster();
    _wind_fs_mount_init();
    wind_file_set_current_path(FS_CUR_PATH);
    return W_ERR_OK;
}

w_fs_s *wind_fs_get(char *name)
{
    w_fs_s *fs;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&fslist)
    {
        fs = NODE_TO_FS(dnode);
        if(wind_strcmp(name,fs->name) == 0)
        {
            wind_enable_switch();
            return fs;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_fs_s *wind_fs_get_bypath(const char *path)
{
    w_fs_s *fs,*retfs = W_NULL;
    w_dnode_s *dnode;
    w_int32_t len;
    wind_disable_switch();
    foreach_node(dnode,&fslist)
    {
        fs = NODE_TO_FS(dnode);
        len = wind_strlen(fs->mount_path);
        if(wind_memcmp(path,fs->mount_path,len) == 0)
        {
            if((retfs == W_NULL)||(wind_strlen(retfs->mount_path) < len))
                retfs = fs;
        }
    }
    wind_enable_switch();
    return retfs;
}


w_err_t wind_fs_mount(char *fsname,char *blkname,char *path)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    w_fs_s *fs;
    w_int32_t len;
    err = mount_param_check(fsname,blkname,path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_INVALID);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_MEM);
    blkdev = wind_blkdev_get(blkname);
    len = wind_strlen(path)+1;
    fs->mount_path = wind_malloc(len);
    WIND_ASSERT_RETURN(fs->mount_path != W_NULL,W_ERR_MEM);
    wind_strcpy(fs->mount_path,path);
    fs->blkdev = blkdev;
    if(fs->ops->init)
        fs->ops->init(fs);
    return W_ERR_OK;
}

w_err_t wind_fs_unmount(char *fsname)
{
    w_fs_s *fs;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_INVALID);
    fs->blkdev = W_NULL;
    DNODE_INIT(fs->fsnode);
    wind_free(fs->mount_path);
    fs->mount_path = W_NULL;
    return W_ERR_OK;
}

w_err_t wind_fs_format(w_fs_s *fs)
{
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fs->ops != W_NULL,W_ERR_PTR_NULL);
    if(fs->ops->format != W_NULL)
        err = fs->ops->format(fs);
    return err;
}

#endif
