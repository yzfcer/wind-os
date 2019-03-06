/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_chdev.h
**创   建   人: Jason Zhou
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
#include "wind_chdev.h"
#include "wind_dlist.h"
#include "wind_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_CHDEV_SUPPORT

#define WIND_CHDEV_MAGIC 0x68353D6A

#define F_CHDEV_OPEN (0x01 << 0) //标记chdev对象是否已经打开
#define IS_F_CHDEV_OPEN(chdev) ((chdev->obj.flag & F_CHDEV_OPEN) == F_CHDEV_OPEN)
#define SET_F_CHDEV_OPEN(chdev) (chdev->obj.flag |= F_CHDEV_OPEN)
#define CLR_F_CHDEV_OPEN(chdev) (chdev->obj.flag &= (~F_CHDEV_OPEN))


typedef enum 
{
    CHDEV_COMMON = 0x01,
    CHDEV_LED,
    CHDEV_GPIO,
    CHDEV_BEEP,
    CHDEV_BUTTON,
    CHDEV_USART,
    CHDEV_SPI,
    CHDEV_CAN,
    CHDEV_IIC,
    CHDEV_IIS,
    CHDEV_VIRTUAL,
}w_chdev_e;



typedef struct __w_chdev_s w_chdev_s;
typedef struct __w_chdev_ops_s w_chdev_ops_s;
struct __w_chdev_s
{
    w_obj_s obj;
    w_int16_t devtype;
    w_int16_t devid;
    w_mutex_s *mutex;
    const w_chdev_ops_s *ops;
};


struct __w_chdev_ops_s
{
    w_err_t   (*init)(w_chdev_s *dev);
    w_err_t   (*deinit)(w_chdev_s *dev);
    w_err_t   (*open)(w_chdev_s *dev);
    w_err_t   (*ioctl)(w_chdev_s *dev,w_int32_t cmd,void *param);
    w_int32_t (*read)(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_int32_t (*write)(w_chdev_s *dev,w_uint8_t *buf,w_uint16_t len);
    w_err_t   (*close)(w_chdev_s *dev);
};

#define WIND_CHDEV_DEF(name,devtype,devid,ops) \
    {{(~WIND_CHDEV_MAGIC),name,{W_NULL,W_NULL},0,0},devtype,devid,W_NULL,ops}


w_err_t _wind_chdev_mod_init(void);
w_err_t _register_chdevs(void);

w_err_t wind_chdev_register(w_chdev_s *dev,w_int32_t count);
w_err_t wind_chdev_unregister(w_chdev_s *dev);

w_chdev_s *wind_chdev_get(const char *name);
w_err_t wind_chdev_open(w_chdev_s *dev);
w_err_t wind_chdev_ioctl(w_chdev_s *dev,w_int32_t cmd,void *param);
w_int32_t wind_chdev_read(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len);
w_int32_t wind_chdev_write(w_chdev_s *dev,w_uint8_t *buf,w_int32_t len);
w_err_t wind_chdev_close(w_chdev_s *dev);
w_err_t wind_chdev_print(void);

#endif

#ifdef __cplusplus
}
#endif

#endif //WIND_DEV_H__
