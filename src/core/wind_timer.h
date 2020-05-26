/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_ticktimer.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os software timer module
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TIMER_H__
#define WIND_TIMER_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_TIMER_SUPPORT
#define TIMER_PERIOD 10
#define WIND_TIMER_MAGIC 0x34F574B2

#define F_TIMER_POOL (0x01 << 0) //Mark whether timer object is allocated through memory pool
#define IS_F_TIMER_POOL(timer) ((timer->obj.flag & F_TIMER_POOL) == F_TIMER_POOL)
#define SET_F_TIMER_POOL(timer) (timer->obj.flag |= F_TIMER_POOL)
#define CLR_F_TIMER_POOL(timer) (timer->obj.flag &= (~F_TIMER_POOL))

#define F_TIMER_RUN (0x01 << 1) //Mark whether the timer object is running
#define IS_F_TIMER_RUN(timer) ((timer->obj.flag & F_TIMER_RUN) == F_TIMER_RUN)
#define SET_F_TIMER_RUN(timer) (timer->obj.flag |= F_TIMER_RUN)
#define CLR_F_TIMER_RUN(timer) (timer->obj.flag &= (~F_TIMER_RUN))

#define F_TIMER_REPEAT (0x01 << 2) //Mark whether timer object is triggered repeatedly
#define IS_F_TIMER_REPEAT(timer) ((timer->obj.flag & F_TIMER_REPEAT) == F_TIMER_REPEAT)
#define SET_F_TIMER_REPEAT(timer) (timer->obj.flag |= F_TIMER_REPEAT)
#define CLR_F_TIMER_REPEAT(timer) (timer->obj.flag &= (~F_TIMER_REPEAT))

typedef struct __w_timer_s w_timer_s;
typedef void (*w_timer_fn)(w_timer_s* timer,void *arg);
struct __w_timer_s
{
    w_obj_s obj;       //Basic object information
    w_uint32_t value;  //Timer current value
    w_uint32_t period; //Timer cycle
    w_timer_fn handle; //Timer interrupt processing function
    void *arg;
};

w_err_t _wind_timer_mod_init(void);
void _wind_timer_event(void);
w_timer_s* wind_timer_get(char *name);
w_err_t wind_timer_init(w_timer_s* timer,
                            const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint16_t flag);

w_timer_s* wind_timer_create(const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint16_t flag);
w_err_t wind_timer_start(w_timer_s* timer);
w_err_t wind_timer_reset(w_timer_s* timer);
w_err_t wind_timer_stop(w_timer_s* timer);
w_err_t wind_timer_destroy(w_timer_s* timer);
w_err_t wind_timer_set_period(w_timer_s* timer,w_uint32_t period_ms);
w_err_t wind_timer_setflag(w_timer_s* timer,w_uint16_t flag);
w_err_t wind_timer_clrflag(w_timer_s* timer,w_uint16_t flag);
w_err_t wind_timer_print_list(void);
w_err_t wind_timer_print_detail(void);

#endif // #if WIND_TIMER_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_TICKTIMER_H__

