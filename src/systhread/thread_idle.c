/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : thread_idle.c
** Author      : Jason Zhou
** Last Date   : 2016/04/03 20:24:37
** Description : System idle thread
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
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_thread.h"

static w_stack_t idlestk[THREAD_IDLE_STKSIZE];
static w_err_t thread_idle(w_int32_t argc,char **argv)
{    
    while(1)
    {
        g_core.idle_cnt ++;
    }
    return W_ERR_OK;
}


w_err_t _create_thread_idle(void)	
{
    w_thread_s *thread;
    thread = wind_thread_create("idle",thread_idle,
                    0,W_NULL,idlestk,THREAD_IDLE_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread,F_THREAD_NO_KILL | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,32767);
    return W_ERR_OK;
}

