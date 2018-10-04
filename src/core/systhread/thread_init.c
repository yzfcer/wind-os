/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_init.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 初始化线程，处理那些不必在线程调度前处理的初始化工作
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
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "wind_watchdog.h"
#include "wind_chdev.h"
#include "wind_blkdev.h"
#include "wind_time.h"
#include "wind_debug.h"
#include "wind_os_hwif.h"
#if WIND_FS_SUPPORT
#include "wind_file.h"
#endif

#define INIT_STK_SIZE 256

void wind_enter_thread_hook(void);
#if WIND_STATI_THREAD_SUPPORT
void _create_thread_stati(void);
#endif

#if WIND_DAEMON_THREAD_SUPPORT
void _create_thread_daemon(void);
#endif

void _create_thread_idle(void);

#if WIND_CONSOLE_SUPPORT
w_err_t _create_thread_console(void);
#endif

#if WIND_TIMER_SUPPORT
w_err_t _create_thread_timer(void);
#endif

extern w_err_t wind_main(void);
static w_stack_t initstk[INIT_STK_SIZE];

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
#if WIND_DRVFRAME_SUPPORT
    _wind_chdev_mod_init();
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    _wind_blkdev_mod_init();
#endif
#if WIND_HEAP_SUPPORT
	_wind_heaps_mod_init();
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
#if WIND_DAEMON_THREAD_SUPPORT
    _create_thread_daemon();
#endif
#if WIND_CONSOLE_SUPPORT
    _create_thread_console();
#endif
    wind_main();
    return W_ERR_OK;
}

w_err_t _create_thread_init(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("init",thread_init,
                        0,W_NULL,PRIO_HIGH,initstk,INIT_STK_SIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}
