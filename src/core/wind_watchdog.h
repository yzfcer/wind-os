/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_watchdog.h
**创   建   人: Jason Zhou
**最后修改日期: 2018.08.12
**描        述: 线程粒度的看门狗，主要是监控线程得不到CPU而饿死
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018.08.12
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2018.08.12
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_WATCHDOG_H__
#define WIND_WATCHDOG_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_WATCHDOG_SUPPORT
#define WIND_WATCHDOG_MAGIC 0x33FA81A6

#define WDOG_WARN  0x00
#define WDOG_RESET 0x01

#define F_WATCHDOG_POOL (0x01 << 0)  //Mark whether the watchdog object is allocated through the memory pool
#define IS_F_WATCHDOG_POOL(watchdog) ((watchdog->obj.flag & F_WATCHDOG_POOL) == F_WATCHDOG_POOL)
#define SET_F_WATCHDOG_POOL(watchdog) (watchdog->obj.flag |= F_WATCHDOG_POOL)
#define CLR_F_WATCHDOG_POOL(watchdog) (watchdog->obj.flag &= (~F_WATCHDOG_POOL))

#define F_WATCHDOG_ENABLE (0x01 << 1)  //Mark whether the watchdog object is enabled
#define IS_F_WATCHDOG_ENABLE(watchdog) ((watchdog->obj.flag & F_WATCHDOG_ENABLE) == F_WATCHDOG_ENABLE)
#define SET_F_WATCHDOG_ENABLE(watchdog) (watchdog->obj.flag |= F_WATCHDOG_ENABLE)
#define CLR_F_WATCHDOG_ENABLE(watchdog) (watchdog->obj.flag &= (~F_WATCHDOG_ENABLE))


#define F_WATCHDOG_WARN (0x01 << 2) //Mark whether alarm occurs after watchdog timeout
#define IS_F_WATCHDOG_WARN(watchdog) ((watchdog->obj.flag & F_WATCHDOG_WARN) == F_WATCHDOG_WARN)
#define SET_F_WATCHDOG_WARN(watchdog) (watchdog->obj.flag |= F_WATCHDOG_WARN)
#define CLR_F_WATCHDOG_WARN(watchdog) (watchdog->obj.flag &= (~F_WATCHDOG_WARN))

#define F_WATCHDOG_RESET (0x01 << 3) //Mark whether to restart after watchdog timeout
#define IS_F_WATCHDOG_RESET(watchdog) ((watchdog->obj.flag & F_WATCHDOG_RESET) == F_WATCHDOG_RESET)
#define SET_F_WATCHDOG_RESET(watchdog) (watchdog->obj.flag |= F_WATCHDOG_RESET)
#define CLR_F_WATCHDOG_RESET(watchdog) (watchdog->obj.flag &= (~F_WATCHDOG_RESET))

typedef struct _wind_watchdog
{
    w_obj_s obj;           //Basic object information
    w_int16_t time_max;    //Initialization time value of the watchdog in seconds
    w_int16_t time_cur;    //Current time value of th  watchdog in seconds
	w_thread_s *thread;    //Thread associated with Watchdog
}w_watchdog_s;

w_err_t _wind_watchdog_mod_init(void);
w_watchdog_s *wind_watchdog_get(const char *name);
w_err_t wind_watchdog_init(w_watchdog_s *watchdog,const char *name,w_int16_t timeout_1s);
w_watchdog_s *wind_watchdog_create(const char *name,w_int16_t timeout_1s);
w_err_t wind_watchdog_destroy(w_watchdog_s *watchdog);
w_err_t wind_watchdog_feed(w_watchdog_s *watchdog);
w_err_t wind_watchdog_setflag(w_watchdog_s *watchdog,w_uint16_t flag);
w_err_t wind_watchdog_clrflag(w_watchdog_s *watchdog,w_uint16_t flag);
w_err_t wind_watchdog_print(void);

#endif // #if WIND_WATCHDOG_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_WATCHDOG_H__

