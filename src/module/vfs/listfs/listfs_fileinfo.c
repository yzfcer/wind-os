/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_fileinfo.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块设备的操作接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.04.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.04.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "listfs_fileinfo.h"
#include "listfs.h"
#include "wind_debug.h"
#include "wind_string.h"

static w_err_t fileinfo_read_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk)
{
    w_err_t err;
    w_int32_t cnt;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr != 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    cnt = wind_blkdev_read(blkdev,addr,blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    return W_ERR_OK;
}

static w_err_t fileinfo_write_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk)
{
    w_int32_t cnt;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blk != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(addr > 0,W_ERR_INVALID);
    cnt = wind_blkdev_write(blkdev,addr,blk,1);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    return W_ERR_OK;
}


w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr)
{
    w_int32_t namelen;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    namelen = wind_strlen(name);
    WIND_ASSERT_RETURN(namelen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(namelen < LFILE_NAME_LEN,W_ERR_INVALID);
    wind_memset(info,0,sizeof(lfile_info_s));
    info->magic = LISTFILE_MAGIC;
    wind_strcpy(info->name,name);
    info->parent_addr = parent_addr;
    info->self_addr = self_addr;
    info->prevfile_addr = prev_addr;
    info->attr = attr;
    return W_ERR_OK;
}


w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr)
{
    w_err_t err;
    
    w_uint8_t *blk = W_NULL;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    if(addr == 0)
    {
        wind_debug("invalid addr for fileinfo");
        return W_ERR_INVALID;
    }

    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        err = fileinfo_read_block(blkdev,addr,blk);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkdata failed");
        tmpinfo = (lfile_info_s*)blk;
        if(tmpinfo->magic != LISTFILE_MAGIC)
        {
            err = W_ERR_INVALID;
            break;
        }
        wind_memcpy(info,tmpinfo,sizeof(lfile_info_s));
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        err = fileinfo_read_block(blkdev,info->self_addr,blk);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkdata failed");
        wind_memcpy(blk,info,sizeof(lfile_info_s));
        err = fileinfo_write_block(blkdev,info->self_addr,blk);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkdata failed");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    return fileinfo_read(info,blkdev,info->parent_addr);
}

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    return fileinfo_read(info,blkdev,info->nextfile_addr);
}

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    return fileinfo_read(info,blkdev,info->headchild_addr);
}

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    return fileinfo_read(info,blkdev,info->tailchild_addr);
}


w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    w_addr_t self_addr;
    w_uint8_t *blk = W_NULL;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    if(info->parent_addr == 0)
        return W_ERR_OK;

    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        self_addr = info->self_addr;
        err = fileinfo_read_block(blkdev,info->parent_addr,blk);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"read parent info failed");
        tmpinfo = (lfile_info_s*)blk;
        if(tmpinfo->headchild_addr == 0)
            tmpinfo->headchild_addr = self_addr;
        tmpinfo->tailchild_addr = self_addr;
        cnt = wind_blkdev_write(blkdev,tmpinfo->self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write blkdata failed");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    w_uint8_t *blk = W_NULL;
    w_addr_t self_addr;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    if(info->prevfile_addr == 0)
        return W_ERR_OK;
    do
    {
        err = W_ERR_OK;
        blk = lfs_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        self_addr = info->self_addr;
        cnt = fileinfo_read_block(blkdev,info->prevfile_addr,blk);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"read blkdata failed");
        tmpinfo = (lfile_info_s*)blk;
        tmpinfo->tailchild_addr = self_addr;
        cnt = wind_blkdev_write(blkdev,tmpinfo->self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"write blkdata failed");
    }while(0);
    if(blk != W_NULL)
        lfs_free(blk);
    return err;
}




