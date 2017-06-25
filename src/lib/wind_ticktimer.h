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
#ifndef WIND_TICKTIMER_H__
#define WIND_TICKTIMER_H__

#include "wind_config.h"
#include "wind_types.h"
#include "wind_os_hwif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_TTIMER_SUPPORT > 0

typedef struct tagTicktimer
{
    //HANDLE handler;
    bool_t used;
    bool_t running;
    u32_t count;
    u32_t inittick;
    void (*timercallback)(void);
}ttimer_s,*pttimer_s;

err_t wind_ttimer_init(void);
pttimer_s wind_ttimer_create(u32_t ticks,void (*timercallback)(void),bool_t run);
err_t wind_ttimer_start(pttimer_s pttimer);
err_t wind_ttimer_stop(pttimer_s pttimer);
err_t wind_ttimer_free(pttimer_s pttimer);
err_t wind_ttimer_setticks(pttimer_s pttimer,u32_t ticks);
extern void ticktimerhandler(void);//定时器入口


#endif //#if WIND_TTIMER_SUPPORT > 0
#ifdef __cplusplus
}
#endif

#endif //#define WIND_TICKTIMER_H__

