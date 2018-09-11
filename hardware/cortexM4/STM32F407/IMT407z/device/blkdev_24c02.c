/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: blkdev_24c02.c
**��   ��   ��: �ܽ���
**����޸�����: 2018.03.26
**��        ��: 24C02 EEPROM���豸ע���ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2018.03.26  
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2018.03.26
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "24c02.h"
#if WIND_BLK_DRVFRAME_SUPPORT

w_err_t   at24c02_open(blkdev_s *dev)
{
    dev->blkaddr = 0;
    dev->blksize = 16;
    dev->blkcnt = 16;        
    IIC_Init();
    return W_ERR_OK;
}

w_err_t   at24c02_erase(blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
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

w_err_t   at24c02_eraseall(blkdev_s *dev)
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


w_int32_t at24c02_read(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t cnt;
    w_uint8_t start;
    start = (w_uint8_t)((dev->blkaddr + addr) * dev->blksize);
    cnt = blkcnt * dev->blksize;
    AT24C02_Read((w_uint8_t)start,buf,(w_uint8_t)cnt);
    return blkcnt;
}

w_int32_t at24c02_write(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
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

w_err_t   at24c02_close(blkdev_s *dev)
{
    return W_ERR_OK;
}

const blkdev_ops_s at24c02_ops = 
{
    NULL,
    at24c02_open,
    at24c02_erase,
    at24c02_eraseall,
    at24c02_read,
    at24c02_write,
    at24c02_close
};

blkdev_s at24c02_dev[1] = 
{
    WIND_BLKDEV_DEF("at24c02",0,0,16,16,&at24c02_ops)
};

#endif


