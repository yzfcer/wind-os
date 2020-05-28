/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : dev_led.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : LED device driver interface
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
#include "wind_config.h"
#include "wind_debug.h"
#include "wind_chdev.h"
#include "led.h"
#if WIND_CHDEV_SUPPORT

static w_uint8_t ledst[3] = {0,0,0};
w_err_t   led_open(w_chdev_s *dev)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,W_ERR_OVERFLOW);
    LED_Init(0x01 << id);
    return W_ERR_OK;
}

w_err_t  led_ioctl(w_chdev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return W_ERR_OK;
}

w_int32_t led_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,W_ERR_OVERFLOW);
    buf[0] = ledst[id];
    return 1;
}

w_int32_t led_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    w_int32_t id = dev->devid;
    WIND_ASSERT_RETURN(id >= 0,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(id < 3,W_ERR_OVERFLOW);
    buf[0]?LED_On(id):LED_Off(id);
    ledst[id] = buf[id];
    return 1;
}

w_err_t   led_close(w_chdev_s *dev)
{
    return W_ERR_OK;
}

const w_chdev_ops_s led_ops = 
{
    W_NULL,
    W_NULL,
    led_open,
    led_ioctl,
    led_read,
    led_write,
    led_close
};

w_chdev_s led_dev[3] = 
{
    WIND_CHDEV_DEF("led0",CHDEV_LED,0,&led_ops),
    WIND_CHDEV_DEF("led1",CHDEV_LED,1,&led_ops),
    WIND_CHDEV_DEF("led2",CHDEV_LED,2,&led_ops),
};
#endif

