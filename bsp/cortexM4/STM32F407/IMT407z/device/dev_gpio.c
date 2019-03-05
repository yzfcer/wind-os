/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: dev_gpio.c
**��   ��   ��: Jason Zhou
**����޸�����: 2018.03.26
**��        ��: GPIO�豸�����ӿ��ļ�
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
#include "wind_chdev.h"
#include "wind_debug.h"
#include "led.h"

#if WIND_CHDEV_SUPPORT
static w_uint8_t ledst[3] = {0,0,0};
w_err_t   gpio_open(w_chdev_s *dev)
{
    LED_Init(0x07);
    return W_ERR_OK;
}

w_err_t  gpio_ioctl(w_chdev_s *dev,w_int32_t ctrlpoint,void *param)
{
    return W_ERR_OK;
}

w_int32_t gpio_read(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len)
{
    w_int32_t i;
    for(i = 0;i < 3;i ++)
        buf[i] = ledst[i];
    return 3;
}

w_int32_t gpio_write(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len)
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

w_err_t   gpio_close(w_chdev_s *dev)
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


