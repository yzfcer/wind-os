/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : dev_gpio.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : GPIO device driver interface
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
#include "wind_chdev.h"
#include "wind_debug.h"
#include "led.h"

#if WIND_CHDEV_SUPPORT
static w_uint8_t ledst[3] = {0,0,0};
static w_err_t   gpio_open(w_chdev_s *dev)
{
    LED_Init(0x07);
    return W_ERR_OK;
}

static w_err_t  gpio_ioctl(w_chdev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return W_ERR_OK;
}

static w_int32_t gpio_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < 3;i ++)
        buf[i] = ledst[i];
    return 3;
}

static w_int32_t gpio_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
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

static w_err_t   gpio_close(w_chdev_s *dev)
{
    return W_ERR_OK;
}

const w_chdev_ops_s gpio_ops = 
{
    W_NULL,
    W_NULL,
    gpio_open,
    gpio_ioctl,
    gpio_read,
    gpio_write,
    gpio_close
};

w_chdev_s gpio_dev[1] = 
{
    WIND_CHDEV_DEF("gpio",CHDEV_GPIO,0,&gpio_ops)
};


#endif


