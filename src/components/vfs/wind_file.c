/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? wind_file.c
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
#include "wind_file.h"
#include "wind_fs.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_core.h"
#include "wind_dlist.h"
#include "wind_board_port.h"

#if WIND_FS_SUPPORT
#define NODE_TO_FILE(dnode) (w_file_s*)(((w_uint8_t*)(dnode))-((w_uint32_t)&(((w_file_s*)0)->filenode)))
static w_dlist_s filelist;
WIND_POOL(filepool,WIND_FILE_MAX_NUM,sizeof(w_file_s));

static w_file_s *file_malloc(void)
{
    return wind_pool_malloc(filepool);
}

static w_err_t file_free(w_file_s *file)
{
    return wind_pool_free(filepool,file);
}

w_err_t _wind_file_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(filelist);
    err = wind_pool_create("file",filepool,sizeof(filepool),sizeof(w_file_s));
    return err;
}



w_file_s *wind_file_get(w_fs_s *fs,const char *path)
{
    w_file_s *file;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&filelist)
    {
        file = NODE_TO_FILE(dnode);
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

w_bool_t wind_file_check(const char *path)
{
    w_file_s *file;
    w_uint32_t isdir;
    w_bool_t exist = W_FALSE;
    WIND_ASSERT_RETURN(path != W_NULL,W_FALSE);
    wind_debug("wind_file_check:%s",path);
    if(wind_file_get_bypath(path) != W_NULL)
        return W_TRUE;
    isdir = path[wind_strlen(path)-1]=='/'?1:0;
    file = wind_fopen(path,FMODE_R);
    if((file != W_NULL)&&(file->isdir==isdir))
    {
        exist = W_TRUE;
        wind_fclose(file);
    }
    return exist;
}

w_file_s *wind_file_get_bypath(const char *path)
{
    w_file_s *file;
    w_fs_s *fs;
    char *realpath = W_NULL;
    w_int32_t len,len1;
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    wind_debug("open file:%s",path);
    len = wind_strlen(path);
    WIND_ASSERT_RETURN(len > 0,W_NULL);
    fs = wind_fs_get_bypath(path);
    if(fs == W_NULL)
    {
        wind_error("path:%s NOT exsit.",path);
        return W_NULL;
    }
    len1 = wind_strlen(fs->mount_path);
    realpath = wind_malloc(len-len1+2);
    wind_strcpy(realpath,(const char*)&path[len1-1]);
    
    file = wind_file_get(fs,path);
    wind_free(realpath);
    return file;

}

static w_file_s *wind_file_create(w_fs_s *fs,char *realpath,w_uint16_t fmode,w_uint8_t isdir)
{
    w_file_s *file;
    w_err_t err;
    file = file_malloc();
    WIND_ASSERT_RETURN(file != W_NULL,W_NULL);
    file->mutex = wind_mutex_create(W_NULL);
    if(file->mutex == W_NULL)
    {
        file_free(file);
        return W_NULL;
    }
    file->path = realpath;
    file->subname = W_NULL;
    DNODE_INIT(file->filenode);
    file->ftype = fs->fstype;
    file->fmode = fmode;
    file->isdir = isdir;
    file->fs = fs;
    file->fileobj = W_NULL;
    file->offset = 0;
    //file->fs->ops = fs->ops;
    err = file->fs->ops->open(file,file->fmode);
    if(err != W_ERR_OK)
    {
        wind_error("open file err:%d",err);
        wind_mutex_destroy(file->mutex);
        file_free(file);
        wind_free(realpath);
        return W_NULL;
    }
    else
    {
        wind_disable_switch();
        dlist_insert_tail(&filelist,&file->filenode);
        wind_enable_switch();
        return file;
    }
}

static w_err_t wind_file_destroy(w_file_s *file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->mutex != W_NULL,W_ERR_PTR_NULL);
    //WIND_ASSERT_RETURN(file->subname != W_NULL,W_ERR_PTR_NULL);
    wind_disable_switch();
    dlist_remove(&filelist,&file->filenode);
    wind_enable_switch();
    wind_mutex_destroy(file->mutex);
    wind_free(file->path);
    if(file->subname != W_NULL)
        wind_free(file->subname);
    file_free(file);
    return W_ERR_OK;
}

w_file_s* wind_fopen(char *path,w_uint16_t fmode)
{
    w_file_s *file;
    w_fs_s *fs;
    w_uint8_t isdir;
    char *realpath = W_NULL;
    w_int32_t pathlen,len1;
    w_err_t err;
    WIND_ASSERT_RETURN(path != W_NULL,W_NULL);
    wind_debug("open file:%s",path);
    file = wind_file_get_bypath(path);
    if(file != W_NULL)
    {
        wind_error("file has been opened.");
        wind_free(realpath);
        return W_NULL;
    }
    
    pathlen = wind_strlen(path);
    WIND_ASSERT_RETURN(pathlen > 0,W_NULL);
    fs = wind_fs_get_bypath(path);
    if(fs == W_NULL)
    {
        wind_error("path:%s NOT exsit.",path);
        return W_NULL;
    }
    len1 = wind_strlen(fs->mount_path);
    realpath = wind_malloc(pathlen-len1+2);
    wind_strcpy(realpath,(const char*)&path[len1-1]);
    isdir = path[pathlen-1] == '/'?1:0;
    file = wind_file_create(fs,realpath,fmode, isdir);
    return file;
}

w_err_t wind_fclose(w_file_s *file)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    wind_debug("close file:%s.",file->path);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->close)
        err = file->fs->ops->close(file);
    wind_mutex_unlock(file->mutex);
    wind_file_destroy(file);
    return err;
}

w_err_t wind_fremove(const char *path)
{
    w_err_t err = W_ERR_FAIL;
    w_file_s *file;
    file = wind_file_get_bypath(path);
    if(file == W_NULL)
        file = wind_fopen(path,FMODE_R);
    if(file == W_NULL)
    {
        wind_warn("file %s is NOT exist.",path);
        return W_ERR_OK;
    }
    if(wind_strcmp(path,file->fs->mount_path) == 0)
    {
        wind_error("can not remove fs %s root directory",file->fs->name);
        return W_ERR_FAIL;
    }
    
    if((file != W_NULL)&&(file->fs->ops->close))
        file->fs->ops->close(file);

    wind_debug("remove file:%s",file->path);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->remove)
        err = file->fs->ops->remove(file);
    wind_mutex_unlock(file->mutex);
    dlist_remove(&filelist,&file->filenode);
    wind_mutex_destroy(file->mutex);
    file_free(file);
    wind_free(file->path);
    return err;
}

char* wind_fchild(w_file_s *dir,w_int32_t index)
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
    if(dir->fs->ops->subfile)
        subname = dir->fs->ops->subfile(dir,index);
    wind_mutex_unlock(dir->mutex);
    return subname;
}

w_err_t wind_fseek(w_file_s *file,w_int32_t offset)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(!file->isdir,W_ERR_INVALID);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->seek)
        err = file->fs->ops->seek(file,offset);
    wind_mutex_unlock(file->mutex);
    return err;
}
    
w_err_t wind_frename(w_file_s *file,char *newname)
{
    w_err_t err = W_ERR_FAIL;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newname != W_NULL,W_ERR_PTR_NULL);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->rename)
        err = file->fs->ops->rename(file,newname);
    wind_mutex_unlock(file->mutex);
    return err;
}

w_int32_t wind_ftell(w_file_s *file)
{
    w_int32_t offset = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->ftell)
        offset = file->fs->ops->ftell(file);
    wind_mutex_unlock(file->mutex);
    return offset;
}

w_int32_t wind_fread(w_file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(buff != W_NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->read)
        len = file->fs->ops->read(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_int32_t wind_fwrite(w_file_s *file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,-1);
    WIND_ASSERT_RETURN(buff != W_NULL,-2);
    WIND_ASSERT_RETURN(size > 0,-3);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->write)
        len = file->fs->ops->write(file,buff,size);
    wind_mutex_unlock(file->mutex);
    return len;
}

w_err_t wind_fgets(w_file_s *file,char *buff, w_int32_t maxlen)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(maxlen > 0,W_ERR_INVALID);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->fgets)
        len = file->fs->ops->fgets(file,buff,maxlen);
    wind_mutex_unlock(file->mutex);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

w_err_t wind_fputs(w_file_s *file,char *buff)
{
    w_int32_t len = -1;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_PTR_NULL);
    wind_mutex_lock(file->mutex);
    if(file->fs->ops->fputs)
        len = file->fs->ops->fputs(file,buff);
    wind_mutex_unlock(file->mutex);
    return len > 0?W_ERR_OK:W_ERR_FAIL;
}

#endif
