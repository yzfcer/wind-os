/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_driver.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ϵͳ����������ص�ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DEV_H__
#define WIND_DEV_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dev.h"
#include "wind_dlist.h"
#include "wind_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIND_DEV_MAGIC 0x68353D6A
typedef struct __chdev_s dev_s;
typedef struct __dev_ops_s dev_ops_s;
struct __chdev_s
{
    w_uint32_t magic;
    dnode_s devnode;
    char name[12];
    w_bool_t opened;
    mutex_s *mutex;
    const dev_ops_s *ops;
};

struct __dev_ops_s
{
    w_err_t   (*open)(void);
    w_err_t   (*ioctl)(w_int32_t ctrlpoint,void *param);
    w_int32_t (*read)(w_uint8_t *buf,w_uint16_t len);
    w_int32_t (*write)(w_uint8_t *buf,w_uint16_t len);
    w_err_t   (*close)(void);
};

w_err_t wind_register_dev(dev_s *dev,w_int32_t count);
w_err_t wind_unregister_dev(dev_s *dev);

w_err_t wind_driver_init(void);
dev_s *wind_chdev_get(char *name);
w_err_t wind_chdev_open(dev_s *dev);
w_err_t wind_chdev_ioctl(dev_s *dev,w_int32_t ctrlpoint,void *param);
w_int32_t wind_chdev_read(dev_s *dev,w_uint8_t *buf,w_int32_t len);
w_int32_t wind_chdev_write(dev_s *dev,w_uint8_t *buf,w_int32_t len);
w_err_t wind_chdev_close(dev_s *dev);

#ifdef __cplusplus
}
#endif


#endif

