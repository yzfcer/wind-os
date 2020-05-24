/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : list_dev.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : 字符设备注册文件
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
#if WIND_CHDEV_SUPPORT

extern w_chdev_s gpio_dev[1];
extern w_chdev_s led_dev[3];
extern w_chdev_s beep_dev[1];

w_err_t _wind_register_chdevs(void)
{
    wind_chdev_register(gpio_dev,1);
    wind_chdev_register(led_dev,3);
    wind_chdev_register(beep_dev,1);
    return W_ERR_OK;
}

#endif

