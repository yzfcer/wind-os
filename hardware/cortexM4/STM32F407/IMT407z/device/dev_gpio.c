/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: dev_gpio.c
**创   建   人: 周江村
**最后修改日期: 2018.03.26
**描        述: GPIO设备驱动接口文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018.03.26  
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2018.03.26
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_dev.h"
#include "wind_debug.h"
#include "led.h"

#if WIND_DRVFRAME_SUPPORT
static w_uint8_t ledst[3] = {0,0,0};
w_err_t   gpio_open(dev_s *dev)
{
    LED_Init(0x07);
    return ERR_OK;
}

w_err_t  gpio_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return ERR_OK;
}

w_int32_t gpio_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    w_int32_t i;
    for(i = 0;i < 3;i ++)
        buf[i] = ledst[i];
    return 3;
}

w_int32_t gpio_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    w_int32_t i;
    for(i = 0;i < 3;i ++)
    {
        if(ledst[i] != buf[i])
        {
            buf[i]?LED_On(i):LED_Off(i);
            ledst[i] = buf[i];
        }
    }
    return 0;
}

w_err_t   gpio_close(dev_s *dev)
{
    return ERR_OK;
}

const dev_ops_s gpio_ops = 
{
    NULL,
    gpio_open,
    gpio_ioctl,
    gpio_read,
    gpio_write,
    gpio_close
};

dev_s gpio_dev[1] = 
{
    WIND_DEV_DEF("gpio",0,&gpio_ops)
};


#endif


