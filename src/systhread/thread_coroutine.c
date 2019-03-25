/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_coroutine.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 协程调度线程
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
#include "wind_coroutine.h"
#include "wind_debug.h"

#if WIND_COROUTINE_SUPPORT
static w_stack_t corout_stack[THREAD_COROUT_STKSIZE];
static w_err_t coroute_main(w_int32_t argc,char **argv)
{
    w_err_t err;
    while(W_TRUE)
    {
        err= _wind_coroutine_dispatch();
        if(err != W_ERR_OK)
            wind_thread_sleep(1);
    }
}

w_err_t _create_coroutine_thread(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("coroutine",coroute_main,0,W_NULL,corout_stack,THREAD_COROUT_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("coroutine") == W_NULL)
        wind_daemon_create("coroutine",_create_coroutine_thread);
#endif
    return W_ERR_OK;
}
#endif

