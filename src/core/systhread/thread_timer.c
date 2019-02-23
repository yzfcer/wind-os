/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_timer.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 定时器线程，处理系统的定时事件
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_daemon.h"
#include "wind_timer.h"
#include "wind_debug.h"

#if WIND_TIMER_SUPPORT
#define TIMER_STK_SIZE 256

static w_stack_t timerstk[TIMER_STK_SIZE];
static w_err_t thread_timer(w_int32_t argc,char **argv)
{
    while(1)
    {
        wind_thread_sleep(10);
        _wind_timer_event();
    }
}

w_err_t _create_thread_timer(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("timer",thread_timer,
                     0,W_NULL,PRIO_HIGH,timerstk,TIMER_STK_SIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,3);
    wind_daemon_create("timer",_create_thread_timer);
    return W_ERR_OK;
}
#endif

