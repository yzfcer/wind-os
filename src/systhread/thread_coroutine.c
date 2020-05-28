/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : thread_coroutine.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : coroutine scheduler thread
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
    return W_ERR_OK;
}

w_err_t _create_coroutine_thread(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("coroutine",coroute_main,0,(char**)W_NULL,corout_stack,THREAD_COROUT_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("coroutine") == W_NULL)
        wind_daemon_create("coroutine",_create_coroutine_thread);
#endif
    return W_ERR_OK;
}
#endif

