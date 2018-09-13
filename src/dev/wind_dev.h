/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_dev.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 字符设备标准化API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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
typedef struct __w_chdev_s w_chdev_s;
typedef struct __w_chdev_ops_s w_chdev_ops_s;
struct __w_chdev_s
{
    w_uint32_t magic;
    char name[12];
    w_int8_t devid;
    w_bool_t opened;
    w_dnode_s devnode;
    w_mutex_s *mutex;
    const w_chdev_ops_s *ops;
};

struct __w_chdev_ops_s
{
    w_err_t   (*init)(w_chdev_s *dev);
    w_err_t   (*open)(w_chdev_s *dev);
    w_err_t   (*ioctl)(w_chdev_s *dev,w_int32_t cmd,void *param);
    w_int32_t (*read)(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_int32_t (*write)(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_err_t   (*close)(w_chdev_s *dev);
};

#define WIND_DEV_DEF(name,devid,ops) {WIND_DEV_MAGIC,name,devid,W_FALSE,{W_NULL,W_NULL},W_NULL,ops}

w_err_t _wind_dev_mod_init(void);
w_err_t _register_devs(void);

w_err_t wind_dev_register(w_chdev_s *dev,w_int32_t count);
w_err_t wind_dev_unregister(w_chdev_s *dev);

w_chdev_s *wind_dev_get(char *name);
w_err_t wind_dev_open(w_chdev_s *dev);
w_err_t wind_dev_ioctl(w_chdev_s *dev,w_int32_t cmd,void *param);
w_int32_t wind_dev_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len);
w_int32_t wind_dev_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len);
w_err_t wind_dev_close(w_chdev_s *dev);
w_err_t wind_dev_print(w_dlist_s *list);

#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_DEV_H__
