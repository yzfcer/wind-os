/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stat.c
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
#include "wind_config.h"
#include "wind_stati.h"
#include "wind_string.h"
#include "wind_debug.h"

#if WIND_STAT_SUPPORT > 0

stat_s G_STAT[STAT_CNT];
void wind_stat_init(void)
{
    WIND_INFO("statistics initializing...\r\n");
    wind_memset(G_STAT,0,sizeof(G_STAT));
    G_STAT[STAT_PROC].tot = WIND_THREAD_MAX_NUM;
    wind_memcpy(G_STAT[STAT_PROC].name,"proc",4);
#if WIND_LOCK_SUPPORT > 0
    G_STAT[STAT_LOCK].tot = WIND_LOCK_NUM;
    wind_memcpy(G_STAT[STAT_LOCK].name,"lock",4);
#endif
#if WIND_SEM_SUPPORT > 0
    G_STAT[STAT_SEM].tot = WIND_SEM_MAX_NUM;
    wind_memcpy(G_STAT[STAT_SEM].name,"sem",3);
#endif
#if WIND_PIPE_SUPPORT > 0
    G_STAT[STAT_PIPE].tot = WIND_PIPE_MAX_NUM;
    wind_memcpy(G_STAT[STAT_PIPE].name,"pipe",4);
#endif
#if WIND_MESSAGE_SUPPORT > 0
    G_STAT[STAT_MESSAGE].tot = WIND_MESSAGE_MAX_NUM;
    wind_memcpy(G_STAT[STAT_MESSAGE].name,"message",7);
    G_STAT[STAT_MBOX].tot = WIND_MBOX_MAX_NUM;
    wind_memcpy(G_STAT[STAT_MBOX].name,"mbox",4);
#endif
#if WIND_TIMER_SUPPORT > 0
    G_STAT[STAT_TIMER].tot = WIND_TTIMER_MAX_NUM;
    wind_memcpy(G_STAT[STAT_TIMER].name,"timer",6);
#endif
    
    G_STAT[STAT_STACK].tot = WIND_STK_MAX_NUM;
    wind_memcpy(G_STAT[STAT_STACK].name,"stack",6);
}

#endif
