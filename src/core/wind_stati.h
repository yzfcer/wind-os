/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stat.h
**创   建   人: 周江村
**最后修改日期: wind os的资源统计相关的代码
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.08.13
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STAT_H__
#define WIND_STAT_H__
#include "wind_type.h"
#include "wind_config.h"


#ifdef __cplusplus
extern "C" {
#endif

#if WIND_STAT_SUPPORT > 0
typedef enum __stat_e
{
    IDX_THREAD,
#if WIND_LOCK_SUPPORT > 0
    IDX_LOCK,
#endif
#if WIND_SEM_SUPPORT > 0
    IDX_SEM,
#endif
#if WIND_PIPE_SUPPORT > 0
    IDX_PIPE,
#endif
#if WIND_MESSAGE_SUPPORT > 0
    IDX_MSG,
    IDX_MBOX,
#endif
#if WIND_TIMER_SUPPORT > 0
    IDX_TIMER,
#endif
    IDX_STACK,
    IDX_CNT
}stat_e;

#define STAT_NAME_LEN 12
typedef struct __stat_s
{
    w_int8_t name[STAT_NAME_LEN];
    w_uint32_t tot;
    w_uint32_t used;
    w_uint32_t max;
    w_uint32_t err;
}stat_s,*pstat_s;

extern stat_s g_stati[IDX_CNT];
#define WIND_STAT_INC(stat_type) g_stati[stat_type].used++
#define WIND_STAT_MINUS(stat_type) g_stati[stat_type].used--

#define WIND_STAT_MAX(stat_type) \
        do{if(g_stati[stat_type].used > g_stati[stat_type].max) \
        g_stati[stat_type].max = g_stati[stat_type].used;}while(0)


void wind_stat_init(void);
#else
#define wind_stat_init()
#define WIND_STAT_INC(stat_type)
#define WIND_STAT_MINUS(stat_type)
#define WIND_STAT_MAX(stat_type)
#endif

#ifdef __cplusplus
}
#endif


#endif

