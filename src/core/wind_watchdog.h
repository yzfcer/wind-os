/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_watchdog.h
**创   建   人: 周江村
**最后修改日期: 2018.08.12
**描        述: 线程粒度的看门狗，主要是监控线程得不到CPU而饿死
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018.08.12
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2018.08.12
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_WATCHDOG_H__
#define WIND_WATCHDOG_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_WATCHDOG_SUPPORT
#define WIND_WATCHDOG_MAGIC 0x33FA81

#define WDOG_WARN  0x00
#define WDOG_RESET 0x01

typedef struct _wind_watchdog
{
    w_uint32_t magic:24;
    w_uint32_t flag;
    const char* name;
    dnode_s watchdognode;
    w_int16_t time_max;    //初始化的信号量的值
    w_int16_t time_cur;    //当前的信号量的值
	thread_s *thread;
}watchdog_s;

w_err_t _wind_watchdog_init(void);
watchdog_s *wind_watchdog_get(const char *name);
watchdog_s *wind_watchdog_create(const char *name,w_uint32_t flag,w_int16_t timeout_1s);
w_err_t wind_watchdog_destroy(watchdog_s *watchdog);
w_err_t wind_watchdog_feed(watchdog_s *watchdog);
w_err_t wind_watchdog_print(dlist_s *list);
#else
#define _wind_watchdog_init() 
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_WATCHDOG_H__

