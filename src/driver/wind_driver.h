/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_driver.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统的驱动的相关的头文件
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
#ifndef WIND_DRIVER_H__
#define WIND_DRIVER_H__

#include "port/wind_config.h"
#include "port/wind_type.h"
#include "driver/wind_driver.h"
#include "core/wind_lock.h"

#ifdef __cplusplus
extern "C" {
#endif


#define WIND_CHER_DEVICE 1
#define WIND_BLOCK_DEVICE 2

typedef struct __chdev_s
{
    struct __chdev_s *next;
    wint8_t name[12];
    HANDLE hdev;
    bool_t sole;//是否独占设备的标志
    bool_t opened;//设备是否打开的标志
    HANDLE lock;//设备互斥锁的句柄
    err_t (*open)(void *param);//打开一个设备，一般是初始化
    err_t (*ioctl)(wuint8_t com,void *param);//配置设备的参数，针对不同的设备可自定义
    u16_t (*write)(wuint8_t *buf,wuint16_t len);//向设备写一个系列字节
    u16_t (*read)(wuint8_t *buf,wuint16_t len);//从设备读取一系列字节
    err_t (*close)(void);//关闭一个设备，打开的逆操作
}chdev_s,*pchdev_s;

void wind_insert_chdev(pchdev_s dev);
err_t wind_remove_chdev(pchdev_s dev);

HANDLE wind_chdev_get(wint8_t name);
err_t wind_chdev_open(HANDLE hdev,bool_t sole,void *param);
err_t wind_chdev_ioctr(HANDLE hdev,wuint8_t com,void *param);
wuint16_t wind_chdev_read(HANDLE hdev,wuint8_t *buf,wuint16_t len);
wuint16_t wind_chdev_write(HANDLE hdev,wuint8_t *buf,wuint16_t len);
err_t wind_chdev_close(HANDLE hdev);

#ifdef __cplusplus
}
#endif


#endif

