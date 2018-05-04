#include "wind_config.h"
#include "wind_fs.h"
#if WIND_FS_SUPPORT

WIND_POOL(filepool,WIND_FILE_MAX_NUM,sizeof(file_s));

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
    file = 
    wind_mutex_lock(file->mutex);
    if(file->ops.open)
    wind_mutex_unlock(file->mutex);
    return NULL;
}

w_err_t wind_file_close(file_s *file)
{
    WIND_ASSERT_RETURN(file != NULL,NULL);
    wind_mutex_lock(file->mutex);
    if(file->ops.open)
    wind_mutex_unlock(file->mutex);
    
    return ERR_OK;
}
w_err_t wind_file_seek(file_s *file,w_int32_t offset)
{
    return ERR_OK;
}
    

w_int32_t wind_file_tell(file_s *file)
{
    return ERR_OK;
}
w_int32_t wind_file_read(file_s *file,char *buff, w_int32_t size)
{
    return ERR_OK;
}
w_int32_t wind_file_write(file_s *file,char *buff, w_int32_t size)
{
    return ERR_OK;
}
w_err_t wind_file_gets(file_s *file,char *buff, w_int32_t maxlen)
{
    return ERR_OK;
}
w_err_t wind_file_puts(file_s *file,char *buff)
{
    return ERR_OK;
}

#endif
