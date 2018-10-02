/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_stati.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 统计线程，目前主要计算CPU占用率
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_debug.h"
#if WIND_STATI_THREAD_SUPPORT

#define STATI_STK_SIZE 256
static w_stack_t statisstk[STATI_STK_SIZE];
static w_err_t thread_stati(w_int32_t argc,char **argv)
{
    w_uint32_t statcnt = 0;
    w_uint32_t stati_ms = 1000;
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
                     0,W_NULL,PRIO_HIGH,statisstk,STATI_STK_SIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,5);
    return W_ERR_OK;
}
#endif
