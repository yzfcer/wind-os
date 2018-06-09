#include "wind_config.h"
#include "wind_fs.h"
#if WIND_FS_SUPPORT

WIND_POOL(filepool,WIND_FILE_MAX_NUM,sizeof(file_s));
file_s *_file_malloc(void)
{
    return wind_pool_malloc(filepool);
}

w_err_t _file_free(file_s *file)
{
    return wind_pool_free(filepool,file);
}

w_err_t _wind_file_init(void)
{
    w_err_t err;
    wind_pool_create("fileobj",filepool,sizeof(filepool),sizeof(file_s));
    return err;
}


file_s *wind_file_get(char *path)
{
    file_s *file;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.filelist)
    {
        fs = DLIST_OBJ(dnode,file_s,filenode);
        if(wind_strcmp(path,path) == 0)
        {
            wind_enable_switch();
            return file;
        }
    }
    wind_enable_switch();
    return NULL;
}

file_s* wind_file_open(const char *path,fmode_e fmode)
{
    file_s *file;
    WIND_ASSERT_RETURN(path != NULL,NULL);
    file = wind_file_get(path);
    if(file != NULL)
    {
        wind_error("file has been opened.");
        return ERR_FAIL;
    }
    file = wind_file_get(path);
    if(file != NULL)
    {
        wind_error("file:%s has been opened.",path);
        return ERR_FILE_HAS_OPENED;
    }
    wind_mutex_lock(file->mutex);
    if(file->ops->open)
        file->ops->
    wind_mutex_unlock(file->mutex);
    return NULL;
}

w_err_t wind_file_close(file_s *file)
{
    w_err_t err = ERR_FAIL;
    WIND_ASSERT_RETURN(file != NULL,ERR_NULL_POINTER);
    wind_mutex_lock(file->mutex);
    if(file->ops->close)
        err = file->ops->close(file);
    wind_mutex_unlock(file->mutex);
    return err;
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

w_int32_t wind_file_read(file_s *file,char *buff, w_int32_t size)
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

w_int32_t wind_file_write(file_s *file,char *buff, w_int32_t size)
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
