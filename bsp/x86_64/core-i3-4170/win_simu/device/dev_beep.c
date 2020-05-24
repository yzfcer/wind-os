/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : dev_beep.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : 蜂鸣器设备驱动接口文件
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
#include "beep.h"
#if WIND_CHDEV_SUPPORT

static w_uint8_t beepst[1];
w_err_t   beep_init(w_chdev_s *dev)
{
    BEEP_Init();
    beepst[0] = 0;
    return W_ERR_OK;
}

w_err_t   beep_open(w_chdev_s *dev)
{
    return W_ERR_OK;
}

w_err_t  beep_ioctl(w_chdev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return W_ERR_OK;
}

w_int32_t beep_read(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    buf[0] = beepst[0];
    return 1;
}

w_int32_t beep_write(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    beepst[0] = buf[0];
    BEEP = buf[0]?1:0;
    return 1;
}

w_err_t   beep_close(w_chdev_s *dev)
{
    return W_ERR_OK;
}

const w_chdev_ops_s beep_ops = 
{
    beep_init,
    W_NULL,
    beep_open,
    beep_ioctl,
    beep_read,
    beep_write,
    beep_close
};

w_chdev_s beep_dev[1] = 
{
    WIND_CHDEV_DEF("beep",0,&beep_ops),
};
#endif

