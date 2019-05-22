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
#include "treefs.h"

#if WIND_FS_SUPPORT
#define NODE_TO_FS(dnode) (w_vfs_s*)(((w_uint8_t*)(dnode))-((w_uint32_t)&(((w_vfs_s*)0)->obj.objnode)))

static w_dlist_s fs_list;
static w_dlist_s fs_ops_list;
static char *fsname[] = {"fs0","fs1","fs2","fs3","fs4"};
WIND_POOL(fspool,WIND_FS_MAX_NUM,sizeof(w_vfs_s));

static w_err_t wind_fstypes_register(void);

w_vfs_s *wind_vfs_create(char *name)
{
    w_err_t err;
    w_vfs_s *vfs;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    vfs = wind_pool_malloc(fspool);
    WIND_ASSERT_RETURN(vfs != W_NULL,W_NULL);
    wind_memset(vfs,0,sizeof(w_vfs_s));
    err = wind_obj_init(&vfs->obj,WIND_VFS_MAGIC,name,&fs_list);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_NULL);
    return vfs;    
}

w_err_t wind_vfs_destroy(w_vfs_s *vfs)
{
    w_err_t err;
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_MEM);
    WIND_ASSERT_RETURN(vfs->obj.magic == WIND_VFS_MAGIC,W_ERR_INVALID);
    err = wind_obj_deinit(&vfs->obj,WIND_VFS_MAGIC,&fs_list);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

static w_err_t vfs_objs_init(void)
{
    w_err_t err;
    w_int32_t i;
    w_vfs_s *vfs;
    WIND_ASSERT_RETURN(sizeof(fsname)/sizeof(char *) >=  WIND_FS_MAX_NUM,W_ERR_FAIL);
    for(i = 0;i < WIND_FS_MAX_NUM;i ++)
    {
        vfs = wind_vfs_create(fsname[i]);
    }
    return W_ERR_OK;
}

static w_err_t mount_param_check(char *fsname,char *fstype,char *blkname,char *path)
{
    w_vfs_s *vfs;
    w_fstype_s *ops;
    w_dnode_s *dnode;
    w_int32_t len;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fstype != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    vfs = wind_vfs_get(fsname);
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_REPEAT);
    ops = wind_fstype_get(fstype);
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_REPEAT);
    len = wind_strlen(path);
    if(len >= FS_MOUNT_PATH_LEN)
    {
        wind_error("mount path is too long");
        return W_ERR_INVALID;
    }
        
    if(wind_strlen(fsname) >= WFS_NAME_LEN)
    {
        wind_error("vfs name is too long");
        return W_ERR_INVALID;
    }
    
    foreach_node(dnode,&fs_list)
    {
        vfs = NODE_TO_FS(dnode);
        if((vfs->mount_path != W_NULL) && 
            (wind_strcmp(path,vfs->mount_path) == 0))
        {
            wind_error("mount path has been used");
            return W_ERR_REPEAT;
        }
            
        if(blkname == W_NULL)
            continue;
        if((vfs->blkdev != W_NULL) && 
            (vfs->blkdev->obj.name != W_NULL) && 
            (wind_strcmp(blkname,vfs->blkdev->obj.name) == 0))
        {
            wind_error("block device has been used");
            return W_ERR_REPEAT;
        }
            
    }
    return W_ERR_OK;
}


w_err_t _wind_vfs_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(fs_list);
    DLIST_INIT(fs_ops_list);
    err = wind_pool_create("vfs",fspool,sizeof(fspool),sizeof(w_vfs_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = vfs_objs_init();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    _wind_file_mod_init();
#if WIND_TREEFS_SUPPORT
    _wind_treefs_mod_init();
#endif
    wind_fstypes_register();
    
    _wind_fs_mount_init();
    wind_file_set_current_path(FS_CUR_PATH);
    return W_ERR_OK;
}

w_vfs_s *wind_vfs_get(char *name)
{
    return (w_vfs_s *)wind_obj_get(name,&fs_list);
}

w_vfs_s *wind_vfs_get_bypath(const char *path)
{
    w_vfs_s *vfs,*retfs = W_NULL;
    w_dnode_s *dnode;
    w_int32_t len;
    wind_disable_switch();
    foreach_node(dnode,&fs_list)
    {
        vfs = NODE_TO_FS(dnode);
        if(!IS_F_VFS_MOUNT(vfs))
            continue;
        len = wind_strlen(vfs->mount_path);
        if(wind_memcmp(path,vfs->mount_path,len) == 0)
        {
            if((retfs == W_NULL)||(wind_strlen(retfs->mount_path) < len))
                retfs = vfs;
        }
    }
    wind_enable_switch();
    return retfs;
}

w_err_t wind_vfs_print(void)
{
    w_dnode_s *dnode;
    w_vfs_s *vfs;
    w_dlist_s *list = &fs_list;
    wind_printf("\r\n\r\nfs mount list:\r\n");
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        vfs = NODE_TO_FS(dnode);
        
        wind_printf("mount %s ",wind_obj_name(&vfs->obj));
        wind_printf("%s ",vfs->fstype?vfs->fstype:"    ");
        wind_printf("%s ",vfs->blkdev?wind_obj_name(&vfs->blkdev->obj):"    ");
        wind_printf("%s\r\n",vfs->mount_path?vfs->mount_path:"    ");
    }
    wind_enable_switch();
    return W_ERR_OK;
}


w_fstype_s *wind_fstype_get(const char *name)
{
    return (w_fstype_s*)wind_obj_get(name,&fs_ops_list);
}


w_err_t wind_fstype_register(w_fstype_s *ops)
{
    w_fstype_s *tmpops;
    WIND_ASSERT_RETURN(ops != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(ops->obj.magic == WIND_FSTYPE_MAGIC,W_ERR_INVALID);
    wind_notice("register vfs ops:%s",wind_obj_name(&ops->obj));
    tmpops = wind_dbgpoint_get(ops->obj.name);
    if(tmpops != W_NULL)
    {
        wind_notice("vfs ops has been registered.\r\n");
        return W_ERR_OK;
    }
    wind_obj_init(&ops->obj,WIND_FSTYPE_MAGIC,ops->obj.name,&fs_ops_list);
    return W_ERR_OK;
}

w_err_t wind_fstype_unregister(w_fstype_s *ops)
{
    w_dnode_s *dnode;
    w_fstype_s *tmpops;
    WIND_ASSERT_RETURN(ops != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(ops->obj.magic == WIND_FSTYPE_MAGIC,W_ERR_INVALID);
    wind_notice("unregister vfs ops:%s",wind_obj_name(&ops->obj));
    wind_obj_deinit(&ops->obj,WIND_FSTYPE_MAGIC,&fs_ops_list);
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    return W_ERR_OK;
}

#if WIND_TREEFS_SUPPORT
extern w_fstype_s treefs_ops;
#endif

static w_err_t wind_fstypes_register(void)
{
#if WIND_TREEFS_SUPPORT
    wind_fstype_register(&treefs_ops);
#endif
    return W_ERR_OK;
}

w_err_t wind_vfs_mount(char *fsname,char *fstype,char *blkname,char *path)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    w_vfs_s *vfs;
    w_int32_t len;
    w_fstype_s *ops;
    err = mount_param_check(fsname,fstype,blkname,path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        vfs = wind_vfs_get(fsname);
        WIND_ASSERT_BREAK(vfs != W_NULL,W_ERR_MEM,"vfs is NOT exist");
        WIND_ASSERT_BREAK(!IS_F_VFS_MOUNT(vfs),W_ERR_INVALID,"vfs has been mounted");
        ops = wind_fstype_get(fstype);
        WIND_ASSERT_BREAK(ops != W_NULL,W_ERR_MEM,"ops is NOT exist");
        blkdev = wind_blkdev_get(blkname);
        WIND_ASSERT_BREAK(blkdev != W_NULL,W_ERR_MEM,"blkdev is NOT exist");
        
        len = wind_strlen(path)+1;
        vfs->mount_path = wind_malloc(len);
        WIND_ASSERT_BREAK(vfs->mount_path != W_NULL,W_ERR_MEM,"malloc mount_path failed");
        wind_strcpy(vfs->mount_path,path);
        
        len = wind_strlen(fstype)+1;
        vfs->fstype = wind_malloc(len);
        WIND_ASSERT_BREAK(vfs->fstype != W_NULL,W_ERR_MEM,"malloc fstype failed");
        wind_strcpy(vfs->fstype,fstype);
        
        vfs->fsobj = W_NULL;
        vfs->blkdev = blkdev;
        vfs->ops = ops;
        if(vfs->ops->init)
            vfs->fsobj = vfs->ops->init(vfs);
        SET_F_VFS_MOUNT(vfs);
    }while(0);
    if(err != W_ERR_OK)
        wind_vfs_unmount(fsname);
    return err;
}

w_err_t wind_vfs_unmount(char *fsname)
{
    w_vfs_s *vfs;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    vfs = wind_vfs_get(fsname);
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_INVALID);
    wind_disable_switch();
    CLR_F_VFS_MOUNT(vfs);
    vfs->blkdev = W_NULL;
    if(vfs->fstype != W_NULL)
        wind_free(vfs->fstype);
    vfs->fstype = W_NULL;
    if(vfs->mount_path != W_NULL)
        wind_free(vfs->mount_path);
    vfs->mount_path = W_NULL;
    vfs->ops = W_NULL;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_vfs_format(w_vfs_s *vfs)
{
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(vfs->ops != W_NULL,W_ERR_PTR_NULL);
    if(vfs->ops->format != W_NULL)
        err = vfs->ops->format(vfs);
    return err;
}

#endif
