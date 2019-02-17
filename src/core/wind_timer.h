/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_ticktimer.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的以tick为精度的代定时器的代码，非精确定时，绝对时间误差为一个tick周期
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TIMER_H__
#define WIND_TIMER_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif

#if WIND_TIMER_SUPPORT
#define TIMER_PERIOD 10
#define WIND_TIMER_MAGIC 0x34F574B2

#define F_TIMER_POOL (0x01 << 0) //标记timer对象是否通过内存池分配
#define IS_F_TIMER_POOL(timer) ((timer->flag & F_TIMER_POOL) == F_TIMER_POOL)
#define SET_F_TIMER_POOL(timer) (timer->flag |= F_TIMER_POOL)
#define CLR_F_TIMER_POOL(timer) (timer->flag &= (~F_TIMER_POOL))

#define F_TIMER_RUN (0x01 << 1) //标记timer对象是否处于启动运行状态
#define IS_F_TIMER_RUN(timer) ((timer->flag & F_TIMER_RUN) == F_TIMER_RUN)
#define SET_F_TIMER_RUN(timer) (timer->flag |= F_TIMER_RUN)
#define CLR_F_TIMER_RUN(timer) (timer->flag &= (~F_TIMER_RUN))

#define F_TIMER_REPEAT (0x01 << 2) //标记timer对象是否重复触发
#define IS_F_TIMER_REPEAT(timer) ((timer->flag & F_TIMER_REPEAT) == F_TIMER_REPEAT)
#define SET_F_TIMER_REPEAT(timer) (timer->flag |= F_TIMER_REPEAT)
#define CLR_F_TIMER_REPEAT(timer) (timer->flag &= (~F_TIMER_REPEAT))

typedef struct __w_timer_s w_timer_s;
typedef void (*w_timer_fn)(w_timer_s* timer,void *arg);
struct __w_timer_s
{
    w_uint32_t magic;
    const char *name;
    w_dnode_s timernode;
    w_uint32_t value;
    w_uint32_t period;
    w_uint32_t flag;
    w_timer_fn handle;
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
                            w_uint32_t flag);

w_timer_s* wind_timer_create(const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint32_t flag);
w_err_t wind_timer_start(w_timer_s* timer);
w_err_t wind_timer_stop(w_timer_s* timer);
w_err_t wind_timer_destroy(w_timer_s* timer);
w_err_t wind_timer_set_period(w_timer_s* timer,w_uint32_t period_ms);
w_err_t wind_timer_set_flag(w_timer_s* timer,w_uint32_t flag);
w_err_t wind_timer_print(void);

#endif //#if WIND_TIMER_SUPPORT
#ifdef __cplusplus
}
#endif

#endif //#define WIND_TICKTIMER_H__

