/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: dev_beep.c
**创   建   人: Jason Zhou
**最后修改日期: 2018.03.26
**描        述: 蜂鸣器设备驱动接口文件
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
    WIND_DEV_DEF("beep",0,&beep_ops),
};
#endif

