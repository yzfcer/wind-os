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


w_err_t hostfs_init(w_hostfs_s *hfs,char *realdir)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(realdir != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(realdir[0] != 0,W_ERR_INVALID);
    hfs->magic = HOSTFS_MAGIC;
    hfs->realdir = wind_salloc(realdir,HP_ALLOCID_HOSTFS);
    WIND_ASSERT_RETURN(hfs->realdir != W_NULL,W_ERR_MEM);
    return W_ERR_OK;
}



w_err_t hostfs_deinit(w_hostfs_s *hfs)
{
    WIND_ASSERT_RETURN(hfs != W_NULL,W_ERR_PTR_NULL);
    if(hfs->realdir != W_NULL)
        wind_free(hfs->realdir);
    return W_ERR_OK;
}

w_err_t hostfs_match(w_blkdev_s *blkdev)
{
    return W_ERR_FAIL;
}

w_hostfile_s* hostfile_open(w_hostfs_s *hfs,const char *path,w_uint8_t mode)
{
    
    return W_NULL;
}


w_err_t hostfile_close(w_hostfile_s* file)
{
    return W_ERR_OK;
}



w_err_t hostfile_remove(w_hostfs_s *hfs,const char *path)
{
    return W_ERR_OK;
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
    return W_TRUE;
}

w_err_t hostfile_seek(w_hostfile_s* file,w_int32_t offset)
{
    return W_ERR_OK;
}

w_int32_t hostfile_ftell(w_hostfile_s* file)
{
    return 0;
}



w_int32_t hostfile_read(w_hostfile_s* file,w_uint8_t *buff, w_int32_t size)
{
    return size;
}


w_int32_t hostfile_write(w_hostfile_s* file,w_uint8_t *buff,w_int32_t size)
{
    return size;
}

w_err_t hostfile_readdir(w_hostfile_s* dir,w_hostfile_s** sub)
{
    return W_ERR_OK;
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

