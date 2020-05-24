/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : dev_stddev.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : 标准输入输出设备驱动接口文件
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
#include "wind_std.h"
#if WIND_CHDEV_SUPPORT


static w_err_t   stddev_init(w_chdev_s *dev)
{
    //_wind_std_init();
    return W_ERR_OK;
}

static w_err_t   stddev_open(w_chdev_s *dev)
{
    return W_ERR_OK;
}


static w_int32_t stddev_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    return wind_std_input(buf,len);
}

static w_int32_t stddev_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len)
{
    return wind_std_output(buf,len);
}

static w_err_t   stddev_close(w_chdev_s *dev)
{
    return W_ERR_OK;
}
const w_chdev_ops_s stddev_ops = 
{
    stddev_init,
    W_NULL,
    stddev_open,
    W_NULL,
    stddev_read,
    stddev_write,
    stddev_close
};

w_chdev_s stddev_dev[1] = 
{
    WIND_CHDEV_DEF("std",CHDEV_BEEP,0,&stddev_ops),
};

#endif

