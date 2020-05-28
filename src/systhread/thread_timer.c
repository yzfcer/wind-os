/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : thread_timer.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : Timer thread, processing soft timer timeout task
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020/05/11 00:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
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

