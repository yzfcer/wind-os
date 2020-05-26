/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_nullblk.c
** Author      : Jason Zhou
** Last Date   : 
** Description : 创建空的块设备的接口
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_nullblk.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#if WIND_BLKDEV_SUPPORT

static w_err_t   nullblk_init(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static w_err_t   nullblk_open(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static w_err_t   nullblk_erase(w_blkdev_s *blkdev,w_addr_t addr,w_int32_t blkcnt)
{
    return W_ERR_OK;
}

static w_err_t   nullblk_eraseall(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}


static w_int32_t nullblk_read(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    return 0;
}

static w_int32_t nullblk_write(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    return 0;
}

static w_err_t   nullblk_close(w_blkdev_s *blkdev)
{
    return W_ERR_OK;
}

static const w_blkdev_ops_s nullblk_ops = 
{
    nullblk_init,
    W_NULL,
    nullblk_open,
    nullblk_erase,
    nullblk_eraseall,
    nullblk_read,
    nullblk_write,
    nullblk_close
};


static w_int16_t inner_devid = 0;
w_err_t wind_nullblk_create(w_blkdev_s  *blkdev, char *name)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL, W_ERR_PTR_NULL);
    wind_memset(blkdev, 0, sizeof(w_blkdev_s));
    blkdev->obj.magic = (~WIND_BLKDEV_MAGIC);
    blkdev->obj.name = name;
    blkdev->devtype = BLKDEV_NULL;
    blkdev->devid  = inner_devid ++;
    blkdev->blkaddr = 0;
    blkdev->blkcnt = 0;
    blkdev->blksize = 0;
    blkdev->user_arg = W_NULL;
    blkdev->ops = &nullblk_ops;
    err = wind_blkdev_register(blkdev,1);    
    WIND_ASSERT_RETURN(err == W_ERR_OK, err);
    return W_ERR_OK;
}

w_err_t wind_nullblk_destroy(w_blkdev_s *blkdev)
{
    w_err_t err;
    WIND_ASSERT_RETURN(blkdev != W_NULL, W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev->obj.magic  == WIND_BLKDEV_MAGIC, W_ERR_INVALID);
    WIND_ASSERT_RETURN(blkdev->devtype == BLKDEV_NULL, W_ERR_INVALID);
    err = wind_blkdev_unregister(blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK, err);
    wind_memset(blkdev, 0, sizeof(w_blkdev_s));
    return W_ERR_OK;
}

#endif


