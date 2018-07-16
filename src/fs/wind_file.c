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
//WIND_POOL(fspool,WIND_FS_MAX_NUM,sizeof(file_s));
WIND_POOL(filepool,WIND_FILE_MAX_NUM,sizeof(file_s));
static char *curpath = NULL;

static file_s *_file_malloc(void)
{
    return wind_pool_malloc(filepool);
}

static w_err_t _file_free(file_s *file)
{
    return wind_pool_free(filepool,file);
}


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
        if(wind_strcmp(path,fs->mount_path) == 0)
            return ERR_OBJ_REPEAT;
        if(wind_strcmp(blkname,fs->blkdev->name) == 0)
            return ERR_OBJ_REPEAT;
    }
    return ERR_OK;
}

w_err_t wind_fs_regster(fs_s *fs,w_int32_t count)
{
    w_int32_t i;
    fs_s *fsi;    
    w_err_t err;
    WIND_ASSERT_RETURN(fs != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(count > 0,ERR_INVALID_PARAM);
    for(i = 0;i < count;i ++)
    {
        WIND_ASSERT_RETURN(fs[i].magic == WIND_FS_MAGIC,ERR_INVALID_PARAM);
        wind_notice("register fs:%s",fs[i].name);
        fsi = wind_fs_get(fs[i].name);
        if(fsi != NULL)
        {
            wind_error("fs %s has been registered.\r\n",fsi->name);
            continue;
        }
        if(fs[i].ops->init)
        {
            err = fs[i].ops->init(&fs[i]);
            if(err != ERR_OK)
            {
                wind_error("fs:%s init failed:%d.",fs[i].name,err);
                continue;
            }
        }
        wind_disable_switch();
        dlist_insert_tail(&g_core.devlist,&fs[i].fsnode);
        wind_enable_switch();    
    }
    return ERR_OK;
}
w_err_t wind_fs_unregster(fs_s *fs)
{
    dnode_s *dnode;
    WIND_ASSERT_RETURN(fs != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(fs->magic == WIND_FS_MAGIC,ERR_INVALID_PARAM);
    wind_notice("unregister fs:%s",fs->name);
    wind_disable_switch();
    dnode = dlist_remove(&g_core.blkdevlist,dnode);
    wind_enable_switch();
    if(dnode == NULL)
    {
        wind_error("fs has NOT been registered.\r\n");
        return ERR_FAIL;
    }
    return ERR_OK;
}

static w_err_t wind_all_fs_regster(void)
{
    extern fs_s fs_treefs[1];
    wind_fs_regster(fs_treefs,1);
    return ERR_OK;
}

w_err_t _wind_fs_init(void)
{
    w_err_t err;
    wind_pool_create("file",filepool,sizeof(filepool),sizeof(file_s));
    wind_all_fs_regster();
    _wind_fs_mount_init();
    wind_file_set_current_path(FS_CUR_PATH);
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

fs_s *wind_fs_get_bypath(char *path)
{
    fs_s *fs,*retfs = NULL;
    dnode_s *dnode;
    w_int32_t len;
    wind_disable_switch();
    foreach_node(dnode,&g_core.filelist)
    {
        fs = DLIST_OBJ(dnode,fs_s,fsnode);
        len = wind_strlen(fs->mount_path);
        if(wind_memcmp(path,fs->name,len) == 0)
        {
            if((retfs == NULL)||(wind_strlen(retfs->mount_path) < len))
                retfs = fs;
        }
    }
    wind_enable_switch();
    return retfs;
}


w_err_t wind_fs_mount(char *fsname,char *blkname,char *path)
{
    w_err_t err;
    blkdev_s *blkdev;
    fs_s *fs;
    w_int32_t len;
    err = mount_param_check(fsname,blkname,path);
    WIND_ASSERT_RETURN(err == ERR_OK,ERR_INVALID_PARAM);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != NULL,ERR_MEM);
    blkdev = wind_blkdev_get(blkname);
    WIND_ASSERT_RETURN(fs != NULL,ERR_MEM);
    len = wind_strlen(path)+1;
    fs->mount_path = wind_malloc(len);
    WIND_ASSERT_RETURN(fs->mount_path != NULL,ERR_MEM);
    wind_strcpy(fs->mount_path,path);
    fs->blkdev = blkdev;
    if(fs->ops->init)
        fs->ops->init(fs);
    wind_disable_switch();
    dlist_insert_tail(&g_core.fslist,&fs->fsnode);
    wind_enable_switch();
    return ERR_OK;
}

w_err_t wind_fs_unmount(char *fsname)
{
    fs_s *fs;
    WIND_ASSERT_RETURN(fsname != NULL,ERR_NULL_POINTER);
    fs = wind_fs_get(fsname);
    WIND_ASSERT_RETURN(fs != NULL,ERR_INVALID_PARAM);
    wind_disable_switch();
    dlist_remove(&g_core.fslist,&fs->fsnode);
    wind_enable_switch();
    fs->blkdev = NULL;
    DNODE_INIT(fs->fsnode);
    wind_free(fs->mount_path);
    fs->mount_path = NULL;
    return ERR_OK;
}

w_err_t wind_fs_format(fs_s *fs)
{
    w_err_t err = ERR_OK;
    WIND_ASSERT_RETURN(fs != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(fs->ops != NULL,ERR_NULL_POINTER);
    if(fs->ops->format != NULL)
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
    if(curpath == NULL)
        wind_free(curpath);
    curpath = wind_malloc(len +1);
    wind_memcpy(curpath,path,len+1);
}

char *wind_file_get_full_path(char *pre_path,char *relative_path,w_uint16_t isdir)
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


file_s *wind_file_get(fs_s *fs,const char *path)
{
    file_s *file;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.filelist)
    {
        file = DLIST_OBJ(dnode,file_s,filenode);
        if((wind_strcmp(path,file->path) == 0) &&
            (file->fs == fs))
        {
            wind_enable_switch();
            return file;
        }
    }
    wind_enable_switch();
    return NULL;
}

w_bool_t wind_file_existing(const char *path)
{
    file_s *file;
    w_bool_t exist = B_FALSE;
    file = wind_file_open(path,FMODE_R);
    if(file != NULL)
    {
        exist = B_TRUE;
        wind_file_close(file);
    }
    return exist;
}
file_s* wind_file_open(const char *path,fmode_e fmode)
{
    file_s *file;
    char *fullpath = NULL,*realpath = NULL;
    fs_s *fs;
    w_int32_t len,len1;
    w_uint16_t isdir;
    w_err_t err;
    WIND_ASSERT_RETURN(path != NULL,NULL);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,NULL);
    isdir = path[len-1] == '/'?1:0;
    fullpath = wind_file_get_full_path(curpath,(char*)path,isdir);
    fs = wind_fs_get_bypath(fullpath);
    if(fs == NULL)
    {
        wind_error("path:%s NOT exsit.",fullpath);
        wind_free(fullpath);
        return NULL;
    }
    len1 = wind_strlen(fs->mount_path);
    realpath = wind_malloc(len-len1+1);
    wind_strcpy(realpath,(const char*)fullpath[len1]);
    
    file = wind_file_get(fs,fullpath);
    if(file != NULL)
    {
        wind_error("file has been opened.");
        wind_free(fullpath);
        wind_free(realpath);
        return NULL;
    }
    file = _file_malloc();
    WIND_ASSERT_RETURN(file != NULL,NULL);
    file->path = realpath;
    DNODE_INIT(file->filenode);
    file->ftype = fs->fstype;
    file->fmode = fmode;
    file->isdir = fullpath[len-1] == '/'?1:0;
    file->fs = fs;
    file->fileobj = NULL;
    file->offset = 0;
    file->mutex = wind_mutex_create(NULL);
    file->ops = fs->ops;
    err = file->ops->open(file,(fmode_e)file->fmode);
    if(err != ERR_OK)
    {
        wind_mutex_destroy(file->mutex);
        _file_free(file);
        wind_free(realpath);
        wind_free(fullpath);
        return NULL;
    }
    else
    {
        dlist_insert_tail(&g_core.filelist,&file->filenode);
        wind_free(fullpath);
        return file;
    }
}

w_err_t wind_file_close(file_s *file)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    wind_mutex_lock(file->mutex);
    if(file->ops->close)
    err = file->ops->close(file);
    wind_mutex_unlock(file->mutex);
    dlist_remove(&g_core.filelist,&file->filenode);
    wind_mutex_destroy(file->mutex);
    _file_free(file);
    wind_free(file->path);
    return err;
}

w_err_t wind_file_remove(file_s *file)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
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

file_s* wind_file_subfile(file_s *file,w_int32_t index)
{
    file_s *subfile = NULL;
    WIND_ASSERT_RETURN(file != NULL,NULL);
    WIND_ASSERT_RETURN(index >= 0,NULL);
    WIND_ASSERT_RETURN(file->isdir != 0,NULL);
    wind_mutex_lock(file->mutex);
    if(file->ops->subfile)
        subfile = file->ops->subfile(file,index);
    wind_mutex_unlock(file->mutex);
    return subfile;
}

w_err_t wind_file_seek(file_s *file,w_int32_t offset)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(offset >= 0,ERR_INVALID_PARAM);
    wind_mutex_lock(file->mutex);
    if(file->ops->seek)
        err = file->ops->seek(file,offset);
    wind_mutex_unlock(file->mutex);
    return err;
}
    
w_err_t wind_file_rename(file_s *file,char *newname)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(newname != NULL,ERR_NULL_POINTER);
    wind_mutex_lock(file->mutex);
    if(file->ops->rename)
        err = file->ops->rename(file,newname);
    wind_mutex_unlock(file->mutex);
    return err;
}

w_int32_t wind_file_tell(file_s *file)
{
    w_int32_t offset = -1;
    WIND_ASSERT_RETURN(file != NULL,-1);
    wind_mutex_lock(file->mutex);
    if(file->ops->ftell)
        offset = file->ops->ftell(file);
    wind_mutex_unlock(file->mutex);
    return offset;
}

w_int32_t wind_file_read(file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != NULL,-1);
    WIND_ASSERT_RETURN(buff != NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->ops->read)
        len = file->ops->read(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_int32_t wind_file_write(file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != NULL,-1);
    WIND_ASSERT_RETURN(buff != NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->ops->write)
        len = file->ops->write(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_err_t wind_file_gets(file_s *file,char *buff, w_int32_t maxlen)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buff != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(maxlen > 0,ERR_INVALID_PARAM);
    wind_mutex_lock(file->mutex);
    if(file->ops->fgets)
        len = file->ops->fgets(file,buff,maxlen);
    wind_mutex_unlock(file->mutex);
    return len > 0?ERR_OK:ERR_FAIL;
}

w_err_t wind_file_puts(file_s *file,char *buff)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(buff != NULL,ERR_NULL_POINTER);
    wind_mutex_lock(file->mutex);
    if(file->ops->fputs)
        len = file->ops->fputs(file,buff);
    wind_mutex_unlock(file->mutex);
    return len > 0?ERR_OK:ERR_FAIL;
}

#endif
