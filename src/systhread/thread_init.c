/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : thread_init.c
** Author      : Jason Zhou
** Last Date   : 2016/04/03 20:24:37
** Description : System initialization thread,handle some actions that need to be initialized in the thread
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
#include "wind_thread.h"
#include "wind_coroutine.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "wind_watchdog.h"
#include "wind_dbgpoint.h"
#include "wind_daemon.h"
#include "wind_user.h"
#include "wind_module.h"
#include "wind_chdev.h"
#include "wind_blkdev.h"
#include "wind_time.h"
#include "wind_debug.h"
#include "wind_dbgpoint.h"
#include "wind_board_port.h"

void wind_enter_thread_hook(void);
#if WIND_STATI_THREAD_SUPPORT
void _create_thread_stati(void);
#endif

#if WIND_DAEMON_SUPPORT
void _create_thread_daemon(void);
#endif
#if WIND_COROUTINE_SUPPORT
    w_err_t _create_coroutine_thread(void);
#endif

void _create_thread_idle(void);


#if WIND_TIMER_SUPPORT
w_err_t _create_thread_timer(void);
#endif

extern w_err_t wind_main(void);
static w_stack_t initstk[THREAD_INIT_STKSIZE];

static void set_idle_cnt(void)
{
    wind_thread_sleep(500);
    g_core.idle_cnt_max = g_core.idle_cnt;
    wind_thread_sleep(1000);
    g_core.idle_cnt_max = g_core.idle_cnt - g_core.idle_cnt_max;
    wind_printf("idle:%d\r\n",g_core.idle_cnt);
    wind_printf("idle count:%d\r\n",g_core.idle_cnt_max);
}


static w_err_t thread_init(w_int32_t argc,char **argv)
{   
    wind_enter_thread_hook();
#if WIND_CHDEV_SUPPORT
    _wind_chdev_mod_init();
#endif
#if WIND_BLKDEV_SUPPORT
    _wind_blkdev_mod_init();
#endif
#if WIND_HEAP_SUPPORT
	_wind_heap_mod_init();
#endif
    
#if WIND_DATETIME_SUPPORT
    _wind_datetime_mod_init();
#endif
#if WIND_WATCHDOG_SUPPORT
    _wind_watchdog_mod_init();
#endif
#if WIND_DBGPOINT_SUPPORT
    _wind_dbgpoint_mod_init();
#endif
#if WIND_COROUTINE_SUPPORT
    _create_coroutine_thread();
#endif

#if WIND_USER_SUPPORT
    _wind_user_mod_init();
#endif

#if WIND_MODULE_SUPPORT
    _wind_module_mod_init();
#endif
    _create_thread_idle();
    set_idle_cnt();
#if WIND_SOFTIRQ_SUPPORT
    _wind_create_thread_softirq();
#endif
#if WIND_TIMER_SUPPORT
    _create_thread_timer();
#endif

#if WIND_STATI_THREAD_SUPPORT
    _create_thread_stati();
#endif
#if WIND_DAEMON_SUPPORT
    _create_thread_daemon();
#endif
    wind_main();
    return W_ERR_OK;
}

w_err_t _create_thread_init(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("init",thread_init,
                        0,W_NULL,initstk,THREAD_INIT_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}
