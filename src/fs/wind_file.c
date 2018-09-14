/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_chdev.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 文件系统标准化API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_file.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_core.h"
#include "wind_var.h"
#include "wind_os_hwif.h"

#if WIND_FS_SUPPORT

WIND_POOL(filepool,WIND_FILE_MAX_NUM,sizeof(w_file_s));
static char *curpath = W_NULL;

w_file_s *_file_malloc(void)
{
    return wind_pool_malloc(filepool);
}

static w_err_t _file_free(w_file_s *file)
{
    return wind_pool_free(filepool,file);
}


static w_err_t mount_param_check(char *fsname,char *blkname,char *path)
{
    //w_blkdev_s *dev;
    w_fs_s *fs;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(fsname != W_NULL,W_ERR_PTR_NULL);
    //WIND_ASSERT_RETURN(blkname != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_PTR_NULL);
    //dev = wind_blkdev_get(blkname);
    //WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_INVALID);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != W_NULL,W_ERR_REPEAT);
    if(wind_strlen(path) >= FS_MOUNT_PATH_LEN)
        return W_ERR_INVALID;
    if(wind_strlen(fsname) >= FS_MOUNT_PATH_LEN)
        return W_ERR_INVALID;
    foreach_node(dnode,&g_core.fslist)
    {
        fs = DLIST_OBJ(dnode,w_fs_s,fsnode);
        if(wind_strcmp(path,fs->mount_path) == 0)
            return W_ERR_REPEAT;
        if(blkname == W_NULL)
            continue;
        if(wind_strcmp(blkname,fs->blkdev->name) == 0)
            return W_ERR_REPEAT;
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
            wind_error("fs %s has been registered.\r\n",fsi->name);
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
        dlist_insert_tail(&g_core.fslist,&fs[i].fsnode);
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
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
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
    w_err_t err;
    wind_pool_create("file",filepool,sizeof(filepool),sizeof(w_file_s));
    wind_all_fs_regster();
    _wind_fs_mount_init();
    wind_file_set_current_path(FS_CUR_PATH);
    return err;
}

w_fs_s *wind_fs_get(char *name)
{
    w_fs_s *fs;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.fslist)
    {
        fs = DLIST_OBJ(dnode,w_fs_s,fsnode);
        if(wind_strcmp(name,fs->name) == 0)
        {
            wind_enable_switch();
            return fs;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_fs_s *wind_fs_get_bypath(char *path)
{
    w_fs_s *fs,*retfs = W_NULL;
    w_dnode_s *dnode;
    w_int32_t len;
    wind_disable_switch();
    foreach_node(dnode,&g_core.fslist)
    {
        fs = DLIST_OBJ(dnode,w_fs_s,fsnode);
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

char *wind_file_get_current_path(void)
{
    return curpath;
}

void wind_file_set_current_path(char *path)
{
    w_int32_t len = wind_strlen(path);
    if(curpath != W_NULL)
        wind_free(curpath);
    curpath = wind_malloc(len +1);
    wind_memcpy(curpath,path,len+1);
}

char *wind_full_path_generate(char *pre_path,char *relative_path,w_uint16_t isdir)
{
    char *path;
    w_int32_t len,len1;
    w_int32_t ap = 0;
    len = wind_strlen(relative_path) + 1;
    if(isdir)
        len += 1;
    if(relative_path[0] == '/')
    {
        path = wind_malloc(len+ap);
        wind_memset(path,0,len+ap);
        wind_strcpy(path,relative_path);
    }
    else
    {
        len1 = wind_strlen(pre_path) + 1;
        len += len1;
        path = wind_malloc(len);
        wind_memset(path,0,len);
        wind_strcpy(path,pre_path);
        if(pre_path[len1-1] != '/')
            path[len1] = '/';
        wind_strcat(path,relative_path);
    }
    
    len = wind_strlen(path);
    if(isdir && (path[len-1] != '/'))
        path[len] = '/';
    return path;
}

w_err_t wind_full_path_release(char *path)
{
    return wind_free(path);
}

w_file_s *wind_file_get(w_fs_s *fs,const char *path)
{
    w_file_s *file;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.filelist)
    {
        file = DLIST_OBJ(dnode,w_file_s,filenode);
        if((wind_strcmp(path,file->path) == 0) &&
            (file->fs == fs))
        {
            wind_enable_switch();
            return file;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_bool_t wind_file_existing(const char *path)
{
    w_file_s *file;
    w_uint32_t isdir;
    w_bool_t exist = W_FALSE;
    WIND_ASSERT_RETURN(path != W_NULL,W_FALSE);
    wind_debug("wind_file_existing:%s",path);
    isdir = path[wind_strlen(path)-1]=='/'?1:0;
    file = wind_file_open(path,FMODE_R);
    if((file != W_NULL)&&(file->isdir==isdir))
    {
        exist = W_TRUE;
        wind_file_close(file);
    }
    return exist;
}
w_file_s* wind_file_open(const char *path,w_fmode_e fmode)
{
    w_file_s *file;
    w_fs_s *fs;
    char *fullpath = W_NULL;
    char *realpath = W_NULL;
    w_int32_t len,len1;
    w_uint16_t isdir;
    w_err_t err;
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    wind_debug("open file:%s",path);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,W_NULL);
    isdir = path[len-1] == '/'?1:0;
    fullpath = wind_full_path_generate(curpath,(char*)path,isdir);
    fs = wind_fs_get_bypath(fullpath);
    if(fs == W_NULL)
    {
        wind_error("path:%s NOT exsit.",fullpath);
        wind_full_path_release(fullpath);
        return W_NULL;
    }
    len1 = wind_strlen(fs->mount_path);
    realpath = wind_malloc(len-len1+1);
    wind_strcpy(realpath,(const char*)&fullpath[len1-1]);
    
    file = wind_file_get(fs,fullpath);
    if(file != W_NULL)
    {
        wind_error("file has been opened.");
        wind_full_path_release(fullpath);
        wind_free(realpath);
        return W_NULL;
    }
    file = _file_malloc();
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    file->path = realpath;
    file->subname = W_NULL;
    DNODE_INIT(file->filenode);
    file->ftype = fs->fstype;
    file->fmode = fmode;
    file->isdir = fullpath[len-1] == '/'?1:0;
    file->fs = fs;
    file->fileobj = W_NULL;
    file->offset = 0;
    file->mutex = wind_mutex_create(W_NULL);
    file->ops = fs->ops;
    err = file->ops->open(file,(w_fmode_e)file->fmode);
    if(err != W_ERR_OK)
    {
        wind_mutex_destroy(file->mutex);
        _file_free(file);
        wind_free(realpath);
        wind_full_path_release(fullpath);
        return W_NULL;
    }
    else
    {
        wind_disable_switch();
        dlist_insert_tail(&g_core.filelist,&file->filenode);
        wind_enable_switch();
        wind_full_path_release(fullpath);
        return file;
    }
}

w_err_t wind_file_close(w_file_s *file)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    wind_debug("close file:%s.",file->path);
    wind_mutex_lock(file->mutex);
    if(file->ops->close)
    err = file->ops->close(file);
    wind_mutex_unlock(file->mutex);
    dlist_remove(&g_core.filelist,&file->filenode);
    wind_mutex_destroy(file->mutex);
    if(file->subname != W_NULL)
        wind_free(file->subname);
    wind_free(file->path);
    _file_free(file);
    return err;
}

w_err_t wind_file_remove(w_file_s *file)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    wind_debug("remove file:%s",file->path);
    wind_mutex_lock(file->mutex);
    if(file->ops->rmfile)
    err = file->ops->rmfile(file);
    wind_mutex_unlock(file->mutex);
    dlist_remove(&g_core.filelist,&file->filenode);
    wind_mutex_destroy(file->mutex);
    _file_free(file);
    wind_free(file->path);
    return err;
}

char* wind_file_subfile(w_file_s *dir,w_int32_t index)
{
    char *subname = W_NULL;
    WIND_ASSERT_RETURN(dir != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(dir >= 0,W_NULL);
    WIND_ASSERT_RETURN(dir->isdir != 0,W_NULL);
    if(dir->subname == W_NULL)
        dir->subname = wind_malloc(WFILE_NAME_LEN);
    WIND_ASSERT_RETURN(dir->subname != W_NULL,W_NULL);
    wind_debug("subfile %d",index);
    wind_mutex_lock(dir->mutex);
    if(dir->ops->subfile)
        subname = dir->ops->subfile(dir,index);
    wind_mutex_unlock(dir->mutex);
    return subname;
}

w_err_t wind_file_seek(w_file_s *file,w_int32_t offset)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(!file->isdir,W_ERR_INVALID);
    wind_mutex_lock(file->mutex);
    if(file->ops->seek)
        err = file->ops->seek(file,offset);
    wind_mutex_unlock(file->mutex);
    return err;
}
    
w_err_t wind_file_rename(w_file_s *file,char *newname)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    wind_mutex_lock(file->mutex);
    if(file->ops->rename)
        err = file->ops->rename(file,newname);
    wind_mutex_unlock(file->mutex);
    return err;
}

w_int32_t wind_file_tell(w_file_s *file)
{
    w_int32_t offset = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    wind_mutex_lock(file->mutex);
    if(file->ops->ftell)
        offset = file->ops->ftell(file);
    wind_mutex_unlock(file->mutex);
    return offset;
}

w_int32_t wind_file_read(w_file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(buff != W_NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->ops->read)
        len = file->ops->read(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_int32_t wind_file_write(w_file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(buff != W_NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->ops->write)
        len = file->ops->write(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_err_t wind_file_gets(w_file_s *file,char *buff, w_int32_t maxlen)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(maxlen > 0,W_ERR_INVALID);
    wind_mutex_lock(file->mutex);
    if(file->ops->fgets)
        len = file->ops->fgets(file,buff,maxlen);
    wind_mutex_unlock(file->mutex);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

w_err_t wind_file_puts(w_file_s *file,char *buff)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    wind_mutex_lock(file->mutex);
    if(file->ops->fputs)
        len = file->ops->fputs(file,buff);
    wind_mutex_unlock(file->mutex);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

#endif
