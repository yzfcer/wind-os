/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: dev_stddev.c
**��   ��   ��: Jason Zhou
**����޸�����: 2018.03.26
**��        ��: ��׼��������豸�����ӿ��ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018.03.26  
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2018.03.26
** �衡��: 
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

