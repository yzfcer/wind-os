/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : list_blkdev.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : Registration entry of block device drivers 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018.03.26  
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2018.03.26
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_blkdev.h"
#include "wind_debug.h"
#if WIND_BLKDEV_SUPPORT
extern w_blkdev_s memblk_dev[3];
extern w_blkdev_s disk_dev[7];
//extern w_blkdev_s spi_flash_dev[2];
//extern w_blkdev_s at24c02_dev[1];
//extern w_blkdev_s null_dev[2];
w_err_t _register_blkdevs(void)
{
    //wind_blkdev_register(&gpio_dev,1);
    //wind_blkdev_register(at24c02_dev,1);
    //wind_blkdev_register(spi_flash_dev,2);
    wind_blkdev_register(memblk_dev,3);
    wind_blkdev_register(disk_dev,7);
    //wind_blkdev_register(null_dev,2);
    return W_ERR_OK;
}
#endif

