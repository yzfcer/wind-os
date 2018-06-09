#include "wind_config.h"
#include "wind_fs.h"
#if WIND_FS_SUPPORT


WIND_POOL(fspool,WIND_FS_MAX_NUM,sizeof(file_s));

static w_err_t mount_param_check(char *fsname,char *blkname,char *path)
{
    blkdev_s *dev;
    fs_s *fs;
    dnode_s *dnode;
    WIND_ASSERT_RETURN(fsname != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(blkname != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(path != NULL,ERR_NULL_POINTER);
    dev = wind_blkdev_get(blkname);
    WIND_ASSERT_RETURN(dev != NULL,ERR_INVALID_PARAM);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs == NULL,ERR_OBJ_REPEAT);
    if(wind_strlen(path) >= FS_MOUNT_PATH_LEN)
        return ERR_INVALID_PARAM;
    if(wind_strlen(fsname) >= FS_MOUNT_PATH_LEN)
        return ERR_INVALID_PARAM;
    foreach_node(dnode,&g_core.fslist)
    {
        fs = DLIST_OBJ(dnode,fs_s,fsnode);
        if(wind_strcmp(path,fs->path) == 0)
            return ERR_OBJ_REPEAT;
        if(wind_strcmp(blkname,fs->dev->name) == 0)
            return ERR_OBJ_REPEAT;
    }
    return ERR_OK;
}

w_err_t _wind_fs_init(void)
{
    w_err_t err;
    wind_pool_create("fs",fspool,sizeof(fspool),sizeof(fs_s));
    treefs_format();
    return err;
}

fs_s *wind_fs_get(char *name)
{
    fs_s *fs;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.fslist)
    {
        fs = DLIST_OBJ(dnode,fs_s,fsnode);
        if(wind_strcmp(name,fs->name) == 0)
        {
            wind_enable_switch();
            return fs;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_err_t wind_fs_mount(char *fsname,char *blkname,char *path)
{
    w_err_t err;
    blkdev_s *dev;
    fs_s *fs;
    err = mount_param_check(fsname,blkname,path);
    WIND_ASSERT_RETURN(err == ERR_OK,ERR_INVALID_PARAM);
    fs = wind_pool_malloc(fspool);
    WIND_ASSERT_RETURN(fs != NULL,ERR_MEM);
    wind_strcpy(fs->name,fsname);
    wind_strcpy(fs->path,path);
    DNODE_INIT(fs->fsnode);
    fs->dev = dev;
    fs->ops = 
    return ERR_OK;
}

w_err_t wind_fs_unmount(char *fsname)
{
    return ERR_OK;
}

w_err_t wind_fs_format(fs_s *fs)
{
    
    return ERR_OK;
}




#endif
