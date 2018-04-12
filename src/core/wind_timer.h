/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_ticktimer.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的以tick为精度的代定时器的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TIMER_H__
#define WIND_TIMER_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif

#if WIND_TIMER_SUPPORT
#define TIMER_PERIOD 10
typedef void (*softimer_fn)(void *arg);
typedef struct tagTicktimer
{
    dnode_s tmrnode;
    w_bool_t running;
    w_uint32_t count;
    w_uint32_t init_count;
    softimer_fn handle;
    void *arg;
}timer_s;

w_err_t wind_timer_init(void);
timer_s* wind_timer_create(w_uint32_t t_ms,softimer_fn func,void *arg,w_bool_t run);
w_err_t wind_timer_start(timer_s* timer);
w_err_t wind_timer_stop(timer_s* timer);
w_err_t wind_timer_destroy(timer_s* timer);
w_err_t wind_timer_set_period(timer_s* timer,w_uint32_t t_ms);
void wind_timer_event(void);


#endif //#if WIND_TIMER_SUPPORT
#ifdef __cplusplus
}
#endif

#endif //#define WIND_TICKTIMER_H__

