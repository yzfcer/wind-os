/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: dev_beep.c
**��   ��   ��: �ܽ���
**����޸�����: 2018.03.26
**��        ��: �������豸�����ӿ��ļ�
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
#include "wind_config.h"
#include "wind_debug.h"
#include "wind_dev.h"
#include "beep.h"
#if WIND_DRVFRAME_SUPPORT

static w_uint8_t beepst[1];
w_err_t   beep_init(dev_s *dev)
{
    BEEP_Init();
    beepst[0] = 0;
    return W_ERR_OK;
}

w_err_t   beep_open(dev_s *dev)
{
    return W_ERR_OK;
}

w_err_t  beep_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return W_ERR_OK;
}

w_int32_t beep_read(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    buf[0] = beepst[0];
    return 1;
}

w_int32_t beep_write(dev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    beepst[0] = buf[0];
    BEEP = buf[0]?1:0;
    return 1;
}

w_err_t   beep_close(dev_s *dev)
{
    return W_ERR_OK;
}

const dev_ops_s beep_ops = 
{
    beep_init,
    beep_open,
    beep_ioctl,
    beep_read,
    beep_write,
    beep_close
};

dev_s beep_dev[1] = 
{
    WIND_DEV_DEF("beep",0,&beep_ops),
};
#endif

