/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : blkdev_24c02.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : 24C02 EEPROM块设备注册文件
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
#include "24c02.h"
#if WIND_BLKDEV_SUPPORT

w_err_t   at24c02_open(w_blkdev_s *dev)
{
    dev->blkaddr = 0;
    dev->blksize = 16;
    dev->blkcnt = 16;        
    IIC_Init();
    return W_ERR_OK;
}

w_err_t   at24c02_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_int32_t i;
    w_uint8_t start;
    w_uint8_t buf[16];
    wind_memset(buf,0xff,16);
    start = (w_uint8_t)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
    {
        AT24C02_Write((w_uint8_t)start,buf,(w_uint8_t)dev->blksize);
        start += dev->blksize;
    }
    return W_ERR_OK;
}

w_err_t   at24c02_eraseall(w_blkdev_s *dev)
{
    w_int32_t cnt;
    w_uint8_t start;
    w_uint8_t buf[16];
    wind_memset(buf,0xff,16);
    start = (w_uint8_t)0;
    cnt = dev->blkcnt * dev->blksize;
    AT24C02_Write((w_uint8_t)start,buf,(w_uint8_t)cnt);
    return W_ERR_OK;
}


w_int32_t at24c02_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt;
    w_uint8_t start;
    start = (w_uint8_t)((dev->blkaddr + addr) * dev->blksize);
    cnt = blkcnt * dev->blksize;
    AT24C02_Read((w_uint8_t)start,buf,(w_uint8_t)cnt);
    return blkcnt;
}

w_int32_t at24c02_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{   
    w_int32_t i;
    w_uint8_t start;
    start = (w_uint8_t)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
    {
        AT24C02_Write((w_uint8_t)start,buf,(w_uint8_t)dev->blksize);
        start += dev->blksize;
    }
    return blkcnt;

}

w_err_t   at24c02_close(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

const w_blkdev_ops_s at24c02_ops = 
{
    W_NULL,
    at24c02_open,
    W_NULL,
    at24c02_erase,
    at24c02_eraseall,
    at24c02_read,
    at24c02_write,
    at24c02_close
};

w_blkdev_s at24c02_dev[1] = 
{
    WIND_BLKDEV_DEF("at24c02",BLKDEV_EEPROM,0,0,16,16,&at24c02_ops)
};

#endif


