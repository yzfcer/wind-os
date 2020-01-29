/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_memblk.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 创建内存块设备的接口
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
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_memblk.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#if WIND_BLKDEV_SUPPORT

static w_err_t   memblk_init(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static w_err_t   memblk_open(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static w_err_t   memblk_erase(w_blkdev_s *blkdev,w_addr_t addr,w_int32_t blkcnt)
{
    w_addr_t start;
    w_int32_t size;
    w_uint8_t *memblk = blkdev->user_arg;
    start = (w_addr_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    wind_memset(&memblk[start],0,size);
    return W_ERR_OK;
}

static w_err_t   memblk_eraseall(w_blkdev_s *blkdev)
{
    w_addr_t start;
    w_int32_t size;
    w_uint8_t *memblk = blkdev->user_arg;
    start = (w_addr_t)((blkdev->blkaddr) * blkdev->blksize);
    size = blkdev->blkcnt * blkdev->blksize;
    wind_memset(&memblk[start],0,size);
    return W_ERR_OK;
}


static w_int32_t memblk_read(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_addr_t start;
    w_int32_t size;
    w_uint8_t *memblk = blkdev->user_arg;
    start = (w_addr_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    //wind_notice("memblk_read:0x%08x,%d",start,size);
    wind_memcpy(buf,&memblk[start],size);
    return blkcnt;
}

static w_int32_t memblk_write(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_addr_t start;
    w_int32_t size;
    w_uint8_t *memblk = blkdev->user_arg;
    start = (w_addr_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    //wind_notice("memblk_write:0x%08x,%d",start,size);
    wind_memcpy(&memblk[start],buf,size);
    return blkcnt;
}

static w_err_t   memblk_close(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static const w_blkdev_ops_s memblk_ops = 
{
    memblk_init,
    W_NULL,
    memblk_open,
    memblk_erase,
    memblk_eraseall,
    memblk_read,
    memblk_write,
    memblk_close
};


static w_int16_t inner_devid = 0;
w_err_t wind_memblk_create(w_blkdev_s  *blkdev,    char *name,void *mem,w_int32_t memsize,w_int32_t blksize)
{
    w_err_t err;
    w_int32_t blkcnt;
    WIND_ASSERT_RETURN(blkdev != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mem != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(memsize > 0, W_ERR_INVALID);
    WIND_ASSERT_RETURN(blksize > 0, W_ERR_INVALID);
    blkcnt = memsize / blksize;
    WIND_ASSERT_RETURN(blkcnt >= 32, W_ERR_INVALID);
    wind_memset(blkdev, 0, sizeof(w_blkdev_s));
    blkdev->obj.magic = (~WIND_BLKDEV_MAGIC);
    blkdev->obj.name = name;
    blkdev->devtype = BLKDEV_RAM;
    blkdev->devid  = inner_devid ++;
    blkdev->blkaddr = 0;
    blkdev->blkcnt = blkcnt;
    blkdev->blksize = blksize;
    blkdev->user_arg = (void*)mem;
    blkdev->ops = &memblk_ops;
    err = wind_blkdev_register(blkdev,1);    
    WIND_ASSERT_RETURN(err == W_ERR_OK, err);
    return W_ERR_OK;
}

w_err_t wind_memblk_destroy(w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic  == WIND_BLKDEV_MAGIC, W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->devtype == BLKDEV_RAM, W_ERR_INVALID);
    err = wind_blkdev_unregister(blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK, err);
    wind_memset(blkdev, 0, sizeof(w_blkdev_s));
    return W_ERR_OK;
}

#endif


