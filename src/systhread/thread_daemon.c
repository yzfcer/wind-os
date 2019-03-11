/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_daemon.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统监视线程，监控一些可能出现异常的系统状态
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
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_daemon.h"

#if WIND_DAEMON_SUPPORT 
static w_stack_t daemonstk[THREAD_DAEMON_STKSIZE];


static w_err_t thread_daemon(w_int32_t argc,char **argv)
{
    (void)argc;
	
    while(1)
    {
        wind_thread_sleep(5000);
        _wind_daemon_period_check();
    }
}

w_err_t _create_thread_daemon(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("daemon",thread_daemon,
                     0,W_NULL,daemonstk,THREAD_DAEMON_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,2);
    wind_thread_setflag(thread,F_THREAD_NO_KILL);
    return W_ERR_OK;
}

#endif

