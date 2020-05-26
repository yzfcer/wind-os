/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : list_dev.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : 设备注册文件
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
#include "wind_string.h"
#include "w25qxx.h"
#if WIND_BLKDEV_SUPPORT

w_err_t   spi_flash_open(w_blkdev_s *dev)
{
    static w_bool_t init_flag = W_FALSE;
    if(wind_strcmp(dev->obj.name,"spi_flash0") == 0)
    {
        dev->blkaddr = 0;
        dev->blksize = 512;
        dev->blkcnt = 16384;        
    }
    else
    {
        dev->blkaddr = 16384;
        dev->blksize = 512;
        dev->blkcnt = 16384;        
    }
    if(!init_flag)
    {
        W25QXX_Init();
        init_flag = W_TRUE;
    }
    return W_ERR_OK;
}

w_err_t   spi_flash_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_Erase_Sector((w_uint32_t)(start + i * dev->blksize));
    return W_ERR_OK;
}

w_err_t   spi_flash_eraseall(w_blkdev_s *dev)
{
    W25QXX_Erase_Chip();
    return W_ERR_OK;
}


w_int32_t spi_flash_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_Read(buf,(w_uint32_t)(start + i * dev->blksize),dev->blksize);
    return W_ERR_OK;
}

w_int32_t spi_flash_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{   
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_SectorWrite(buf,(w_uint32_t)(start + i * dev->blksize),dev->blksize);
    return W_ERR_OK;
}

w_err_t   spi_flash_close(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

const w_blkdev_ops_s spi_flash_ops = 
{
    W_NULL,
    W_NULL,
    spi_flash_open,
    spi_flash_erase,
    spi_flash_eraseall,
    spi_flash_read,
    spi_flash_write,
    spi_flash_close
};

w_blkdev_s spi_flash_dev[2] = 
{
    WIND_BLKDEV_DEF("spi_flash0",BLKDEV_SPIFLASH,0,0,16384,512,&spi_flash_ops),
    WIND_BLKDEV_DEF("spi_flash1",BLKDEV_SPIFLASH,1,16384,16384,512,&spi_flash_ops)
};

#endif


