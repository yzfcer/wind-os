/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: blkdev_null.c
**��   ��   ��: Jason Zhou
**����޸�����: 2018.03.26
**��        ��: 24C02 EEPROM���豸ע���ļ�
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
#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "24c02.h"
#if WIND_BLKDEV_SUPPORT

static w_err_t   null_open(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

static w_err_t   null_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    return W_ERR_OK;
}

static w_err_t   null_eraseall(w_blkdev_s *dev)
{
    return W_ERR_OK;
}


static w_int32_t null_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    return blkcnt;
}

static w_int32_t null_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{   
    return blkcnt;
}

static w_err_t   null_close(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

const w_blkdev_ops_s null_ops = 
{
    W_NULL,
    W_NULL,
    null_open,
    null_erase,
    null_eraseall,
    null_read,
    null_write,
    null_close
};

w_blkdev_s null_dev[1] = 
{
    WIND_BLKDEV_DEF("null",BLKDEV_COMMON,0,0,0,0,&null_ops)
};

#endif


