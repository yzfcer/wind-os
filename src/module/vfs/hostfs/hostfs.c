/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: hostfs.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.09.08
**描        述: hostfs宿主机文件系统主题功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.09.08
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.09.08
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "hostfs.h"
#include "wind_conv.h"
#include "wind_heap.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_crc32.h"
#include "wind_filepath.h"
#if WIND_HOSTFS_SUPPORT


w_err_t _wind_hostfs_mod_init(void)
{
    return W_ERR_OK;
}


void *hostfs_mem_malloc(w_int32_t size)
{
    void *ptr;
    wind_trace("hostfs_mem_malloc:0x%08x,%d",ptr,size);
    ptr = wind_alloc(size,HP_ALLOCID_HOSTFS);
    if(ptr != W_NULL)
        wind_memset(ptr,0,size);
    return ptr;
}

w_err_t hostfs_mem_free(void *ptr)
{
    wind_trace("hostfs_mem_free:0x%08x",ptr);
    if(ptr == W_NULL)
        return W_ERR_OK;
    return wind_free(ptr);
}


w_err_t hostfs_format(w_hostfs_s *hfs,w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}


w_err_t hostfs_init(w_hostfs_s *hfs,char *dir_prefix)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir_prefix != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir_prefix[0] != 0,W_ERR_INVALID);
    hfs->magic = HOSTFS_MAGIC;
    hfs->dir_prefix = wind_salloc(dir_prefix,HP_ALLOCID_HOSTFS);
    WIND_ASSERT_RETURN(hfs->dir_prefix != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}



w_err_t hostfs_deinit(w_hostfs_s *hfs)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    if(hfs->dir_prefix != W_NULL)
        wind_free(hfs->dir_prefix);
    return W_ERR_OK;
}

w_err_t hostfs_match(w_blkdev_s *blkdev)
{
    return W_ERR_FAIL;
}

w_hostfile_s* hostfile_open(w_hostfs_s *hfs,const char *path,w_uint8_t mode)
{
    w_err_t err;
    w_uint8_t isdir;
    hfile_s *hfile = W_NULL;
    char *fullpath = W_NULL;
    w_hostfile_s *hostfile = W_NULL;
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir(path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        hfile = hostapi_file_open(fullpath,mode);
        WIND_ASSERT_BREAK(hfile != W_NULL,W_ERR_FAIL,"open hfile failed");
        hostfile = hostapi_mem_malloc(sizeof(w_hostfile_s));
        WIND_ASSERT_BREAK(hostfile != W_NULL,W_ERR_MEM,"alloc hostfile failed");
        hostfile->magic = HOSTFILE_MAGIC;
        hostfile->hfs = hfs;
        hostfile->hfile = hfile;
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    if(err != W_ERR_OK)
    {
        if(hfile)
            wind_free(hfile);
        if(hostfile)
            wind_free(hostfile);
        hostfile = W_NULL;
    }
    return hostfile;
}


w_err_t hostfile_close(w_hostfile_s* file)
{
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    if(file->hfile)
        hostapi_file_close(file->hfile);
    wind_free(file);
    return W_ERR_OK;
}



w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_uint8_t isdir;
    char *fullpath = W_NULL;
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir(path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        err = hostapi_file_remove(fullpath);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"remove hfile failed");
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err;
}


w_err_t hostfile_set_attr(w_hostfile_s* file,w_uint8_t attr)
{
    return W_ERR_OK;
}

w_err_t hostfile_get_attr(w_hostfile_s* file,w_uint8_t *attr)
{
    return W_ERR_OK;
}


w_bool_t hostfile_existing(w_hostfs_s *hfs,const char *path)
{
    w_err_t err;
    w_uint8_t isdir;
    char *fullpath = W_NULL;
    do
    {
        err = W_ERR_OK;
        isdir = wind_filepath_isdir(path);
        fullpath = wind_filepath_generate(hfs->dir_prefix,path,isdir);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"get full path failed");
        err = hostapi_file_exist(fullpath);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"check hfile failed");
    }while(0);
    if(fullpath)
        wind_free(fullpath);
    return err == W_ERR_OK ? W_TRUE : W_FALSE;
}

w_err_t hostfile_seek(w_hostfile_s* file,w_int32_t offset)
{
    w_err_t err;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        err = hostapi_file_seek(file->hfile,offset);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"seek hfile failed");
    }while(0);
    return err;
}

w_int32_t hostfile_ftell(w_hostfile_s* file)
{
    w_err_t err;
    w_int32_t offset = -1;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        offset = hostapi_file_tell(file->hfile);
        WIND_ASSERT_BREAK(offset >= 0,err,"tell hfile failed");
    }while(0);
    return offset;
}



w_int32_t hostfile_read(w_hostfile_s* file,w_uint8_t *buff, w_int32_t size)
{
    w_int32_t rdsize = 0;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    rdsize = hostapi_file_read(file->hfile,buff,size);
    return rdsize;
}


w_int32_t hostfile_write(w_hostfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    w_int32_t wrsize = 0;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(file->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    wrsize = hostapi_file_read(file->hfile,buff,size);
    return wrsize;
}

w_err_t hostfile_readdir(w_hostfile_s* dir,w_hostfile_s** sub)
{
    w_err_t err;
    hfile_s *hfile;
    w_hostfile_s *tmp = *sub;
    WIND_ASSERT_RETURN(dir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(sub != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(dir->magic != HOSTFILE_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(tmp != W_NULL,W_ERR_PTR_NULL);
    do
    {
        hfile = hostapi_file_readdir(dir->hfile);
        WIND_CHECK_BREAK(hfile != W_NULL,W_ERR_FAIL);
        tmp->magic = HOSTFILE_MAGIC;
        tmp->hfs = dir->hfs;
        tmp->hfile = hfile;
    }while(0);
    return err;
}


w_err_t hostfile_fgets(w_hostfile_s* file,char *buff, w_int32_t maxlen)
{
    return W_ERR_OK;
}

w_err_t hostfile_fputs(w_hostfile_s* file,char *buff)
{
    return W_ERR_OK;
}

#endif

