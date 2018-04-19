/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_dev.h
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

#if WIND_DRVFRAME_SUPPORT

#define WIND_DEV_MAGIC 0x68353D6A
typedef struct __dev_s dev_s;
typedef struct __dev_ops_s dev_ops_s;
struct __dev_s
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
    w_err_t   (*open)(dev_s *dev);
    w_err_t   (*ioctl)(dev_s *dev,w_int32_t cmd,void *param);
    w_int32_t (*read)(dev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_int32_t (*write)(dev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_err_t   (*close)(dev_s *dev);
};
w_err_t _wind_dev_init(void);
w_err_t _register_devs(void);

w_err_t wind_dev_register(dev_s *dev,w_int32_t count);
w_err_t wind_dev_unregister(dev_s *dev);

dev_s *wind_dev_get(char *name);
w_err_t wind_dev_open(dev_s *dev);
w_err_t wind_dev_ioctl(dev_s *dev,w_int32_t cmd,void *param);
w_int32_t wind_dev_read(dev_s *dev,w_uint8_t *buf,w_int32_t len);
w_int32_t wind_dev_write(dev_s *dev,w_uint8_t *buf,w_int32_t len);
w_err_t wind_dev_close(dev_s *dev);
w_err_t wind_dev_print(dlist_s *list);
#else
#define _wind_dev_init()
#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_DEV_H__
