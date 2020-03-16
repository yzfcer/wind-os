/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: list_dev.c
**创   建   人: Jason Zhou
**最后修改日期: 2018.03.26
**描        述: 块设备驱动注册文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018.03.26  
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2018.03.26
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_blkdev.h"
#include "wind_debug.h"
#if WIND_BLKDEV_SUPPORT
//extern w_blkdev_s memblk_dev[1];
//extern w_blkdev_s spi_flash_dev[2];
//extern w_blkdev_s at24c02_dev[1];
//extern w_blkdev_s null_dev[1];
w_err_t _register_blkdevs(void)
{
    //wind_blkdev_register(&gpio_dev,1);
    //wind_blkdev_register(at24c02_dev,1);
    //wind_blkdev_register(spi_flash_dev,2);
    //wind_blkdev_register(memblk_dev,1);
    //wind_blkdev_register(null_dev,1);
    return W_ERR_OK;
}
#endif

