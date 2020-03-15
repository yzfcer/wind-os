/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? thread_timer.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? 定时器线程，处理系统的定时事�?
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_daemon.h"
#include "wind_timer.h"
#include "wind_debug.h"

#if WIND_TIMER_SUPPORT

static w_stack_t timerstk[THREAD_TIMER_STKSIZE];
static w_err_t thread_timer(w_int32_t argc,char **argv)
{
    while(1)
    {
        wind_thread_sleep(10);
        _wind_timer_event();
    }
    return W_ERR_OK;
}

w_err_t _create_thread_timer(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("timer",thread_timer,
                     0,W_NULL,timerstk,THREAD_TIMER_STKSIZE);    
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread, F_THREAD_DAEMON | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,3);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("timer") == W_NULL)
        wind_daemon_create("timer",_create_thread_timer);
#endif
    return W_ERR_OK;
}
#endif

