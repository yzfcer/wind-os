/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : thread_daemon.c
** Author      : Jason Zhou
** Last Date   : 2016/04/03 20:24:37
** Description : Background monitoring thread
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
    return W_ERR_OK;
}

w_err_t _create_thread_daemon(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("daemon",thread_daemon,
                     0,W_NULL,daemonstk,THREAD_DAEMON_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread,F_THREAD_NO_KILL | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,2);
    return W_ERR_OK;
}

#endif

