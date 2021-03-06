/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? thread_init.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? 初始化线程，处理那些不必在线程调度前处理的初始化工作
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
#include "wind_core.h"
#include "wind_thread.h"
#include "wind_coroutine.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "wind_watchdog.h"
#include "wind_dbgpoint.h"
#include "wind_daemon.h"
#include "wind_user.h"
#include "wind_chdev.h"
#include "wind_blkdev.h"
#include "wind_time.h"
#include "wind_debug.h"
#include "wind_dbgpoint.h"
#include "wind_board_port.h"
#include "db_if.h"
#if WIND_FS_SUPPORT
#include "wind_fs.h"
#endif 


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

#if WIND_SHELL_SUPPORT
static w_stack_t ctrlstk[THREAD_SHELL_STKSIZE];//�������ջ
extern w_err_t thread_shell(w_int32_t argc,char **argv);

w_err_t _create_thread_shell(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("shell",thread_shell,
               0,W_NULL,ctrlstk,THREAD_SHELL_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,32760);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("shell") == W_NULL)
        wind_daemon_create("shell",_create_thread_shell);
#endif
    return W_ERR_OK;
}

#endif

#if WIND_TIMER_SUPPORT
w_err_t _create_thread_timer(void);
#endif

extern w_err_t wind_main(void);
static w_stack_t initstk[THREAD_INIT_STKSIZE];

static void set_idle_cnt(void)
{
    wind_thread_sleep(500);
    IDLE_CNT_PER_SEC = g_core.idle_cnt;
    wind_thread_sleep(1000);
    IDLE_CNT_PER_SEC = g_core.idle_cnt - IDLE_CNT_PER_SEC;
    wind_printf("idle:%d\r\n",g_core.idle_cnt);
    wind_printf("idle count:%d\r\n",IDLE_CNT_PER_SEC);
}

extern w_err_t treefs_format(void);

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
#if WIND_FS_SUPPORT
    _wind_fs_mod_init();
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
#if WIND_DB_SUPPORT
    _wind_db_mod_init();
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
#if WIND_SHELL_SUPPORT
    _create_thread_shell();
#endif
    _wind_thread_set_usrmode();
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
