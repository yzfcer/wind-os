/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
**�?  �?  �? thread_stati.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? 统计线程，目前主要计算CPU占用�?
**              
**--------------History---------------------------------------------------------------------------------
** 创建�? 
** �? �? v1.0
** 日　�? 
** 描　�? First version
**
**--------------Cureent version-------------------------------------------------------------------------
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
    //w_core_var_s core_var;
    //wind_get_core_var(&core_var)
    while(1)
    {
        statcnt = g_core.idle_cnt;
        wind_thread_sleep(stati_ms);
        statcnt = g_core.idle_cnt - statcnt;
        g_core.cpu_usage = (g_core.idle_cnt_max - statcnt) *100 / g_core.idle_cnt_max;
        if(g_core.cpu_usage > 100)
            g_core.cpu_usage = 0;
    }
    return W_ERR_OK;
}

w_err_t _create_thread_stati(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("statistics",thread_stati,
                     0,W_NULL,statisstk,THREAD_STATI_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread,F_THREAD_NO_KILL | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,5);
    return W_ERR_OK;
}
#endif
