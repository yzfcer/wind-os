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

#if 0
static char *get_type_name(w_fstype_e type)
{
    switch(type)
    {
        case FSTYPE_NONE:return "none";
        case FSTYPE_TREEFS:return "treefs";
        case FSTYPE_LISTFS:return "listfs";
        default:return "undefine";
    }
}
#endif

    
static w_err_t fsobj_init(w_vfs_s *fs,char *name)
{
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    wind_memset(fs,0,sizeof(w_vfs_s));
    return wind_obj_init(&fs->obj,WIND_FS_MAGIC,name,&fs_list);
}

static w_err_t fs_objs_init(void)
{
    w_err_t err;
    w_int32_t i;
    w_vfs_s *fs;
    WIND_ASSERT_RETURN(sizeof(fsname)/sizeof(char *) >=  WIND_FS_MAX_NUM,W_ERR_FAIL);
    for(i = 0;i < WIND_FS_MAX_NUM;i ++)
    {
        fs = wind_pool_malloc(fspool);
        WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_MEM);
        //err = fsobj_init(fs,fsname[i]);
        //WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        wind_memset(fs,0,sizeof(w_vfs_s));
        wind_obj_init(&fs->obj,WIND_FS_MAGIC,fsname[i],&fs_list);
    }
    return W_ERR_OK;
}

static w_err_t mount_param_check(char *fsname,char *fstype,char *blkname,char *path)
{
    w_vfs_s *fs;
    w_fstype_s *ops;
    w_dnode_s *dnode;
    w_int32_t len;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fstype != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    fs = wind_vfs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_REPEAT);
    ops = wind_fstype_get(fstype);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_REPEAT);
    len = wind_strlen(path);
    if(len >= FS_MOUNT_PATH_LEN)
    {
        wind_error("mount path is too long");
        return W_ERR_INVALID;
    }
        
    if(wind_strlen(fsname) >= WFS_NAME_LEN)
    {
        wind_error("fs name is too long");
        return W_ERR_INVALID;
    }
    
    foreach_node(dnode,&fs_list)
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


w_err_t _wind_vfs_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(fs_list);
    DLIST_INIT(fs_ops_list);
    err = wind_pool_create("fs",fspool,sizeof(fspool),sizeof(w_vfs_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = fs_objs_init();
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    
    _wind_file_mod_init();
    _wind_treefs_mod_init();
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
    w_vfs_s *fs,*retfs = W_NULL;
    w_dnode_s *dnode;
    w_int32_t len;
    wind_disable_switch();
    foreach_node(dnode,&fs_list)
    {
        fs = NODE_TO_FS(dnode);
        if(!IS_F_VFS_MOUNT(fs))
            continue;
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

w_err_t wind_vfs_print(void)
{
    w_dnode_s *dnode;
    w_vfs_s *fs;
    w_dlist_s *list = &fs_list;
    wind_printf("\r\n\r\nfs mount list:\r\n");
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        fs = NODE_TO_FS(dnode);
        wind_printf("mount:%s,",wind_obj_name(&fs->obj));
        wind_printf("fstype:%s,",fs->fstype?fs->fstype:"none");
        wind_printf("blkdev:%s,",fs->blkdev?wind_obj_name(&fs->blkdev->obj):"none");
        wind_printf("mount path:%s\r\n",fs->mount_path?fs->mount_path:"none");
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
    wind_notice("register fs ops:%s",wind_obj_name(&ops->obj));
    tmpops = wind_dbgpoint_get(ops->obj.name);
    if(tmpops != W_NULL)
    {
        wind_notice("fs ops has been registered.\r\n");
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
    wind_notice("unregister fs ops:%s",wind_obj_name(&ops->obj));
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
    vfs = wind_vfs_get(fsname);
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_MEM);
    ops = wind_fstype_get(fstype);
    WIND_ASSERT_RETURN(ops != W_NULL,W_ERR_MEM);
    blkdev = wind_blkdev_get(blkname);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_MEM);
    
    len = wind_strlen(path)+1;
    vfs->mount_path = wind_malloc(len);
    WIND_ASSERT_RETURN(vfs->mount_path != W_NULL,W_ERR_MEM);
    wind_strcpy(vfs->mount_path,path);
    vfs->fstype = fstype;
    vfs->fsobj = W_NULL;
    vfs->blkdev = blkdev;
    vfs->ops = ops;
    if(vfs->ops->init)
        vfs->fsobj = vfs->ops->init(vfs);
    SET_F_VFS_MOUNT(vfs);
    return W_ERR_OK;
}

w_err_t wind_vfs_unmount(char *fsname)
{
    w_vfs_s *fs;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    fs = wind_vfs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_INVALID);
    wind_disable_switch();
    fs->blkdev = W_NULL;
    fs->fstype = W_NULL;
    fs->mount_path = W_NULL;
    fs->ops = W_NULL;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_vfs_format(w_vfs_s *fs)
{
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fs->ops != W_NULL,W_ERR_PTR_NULL);
    if(fs->ops->format != W_NULL)
        err = fs->ops->format(fs);
    return err;
}

#endif
