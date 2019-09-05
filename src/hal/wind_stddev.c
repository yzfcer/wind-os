/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: dev_stddev.c
**创   建   人: Jason Zhou
**最后修改日期: 2018.03.26
**描        述: 标准输入输出设备驱动接口文件
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
#include "wind_std.h"
#if WIND_CHDEV_SUPPORT


static w_err_t   stddev_init(w_chdev_s *dev)
{
    _wind_std_init();
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

