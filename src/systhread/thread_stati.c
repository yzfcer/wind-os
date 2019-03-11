/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? thread_stati.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? 统计线程，目前主要计算CPU占用�?
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建�? 
** �? �? v1.0
** 日　�? 
** 描　�? 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改�? 
** 日　�? 
** 描　�? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_core.h"
#include "wind_thread.h"
#include "wind_debug.h"
#if WIND_STATI_THREAD_SUPPORT


static w_stack_t statisstk[THREAD_STATI_STKSIZE];
static w_err_t thread_stati(w_int32_t argc,char **argv)
{
    w_uint32_t statcnt = 0;
    w_int32_t stati_ms = 1000;
    while(1)
    {
        statcnt = g_core.idle_cnt;
        wind_thread_sleep(stati_ms);
        statcnt = g_core.idle_cnt - statcnt;
        WIND_CPU_USAGE = (IDLE_CNT_PER_SEC - statcnt) *100 / IDLE_CNT_PER_SEC;
        if(WIND_CPU_USAGE > 100)
            WIND_CPU_USAGE = 0;
    }
}

w_err_t _create_thread_stati(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("statistics",thread_stati,
                     0,W_NULL,statisstk,THREAD_STATI_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,5);
    wind_thread_setflag(thread,F_THREAD_NO_KILL);
    return W_ERR_OK;
}
#endif
