/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_core.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的核心启动和线程调度功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_config.h"
#include "wind_os_hwif.h"

#include "wind_sysinfo.h"
#include "wind_thread.h"
#include "wind_mutex.h"
#include "wind_sem.h"
#include "wind_heap.h"
#include "wind_pool.h"
#include "wind_msgbox.h"
#include "wind_pipe.h"
#include "wind_timer.h"
#include "wind_softirq.h"
#include "wind_daemon.h"
#include "wind_event.h"

#include "wind_core.h"
#include "wind_time.h"
#include "wind_std.h"
#include "wind_debug.h"


extern void wind_thread_switch(void);
extern void wind_interrupt_switch(void);
extern void wind_start_switch(void);
extern int _create_thread_init(void);


w_core_var_s g_core;//wind-os的基本全局参数和各种内核资源的链表头
volatile w_bool_t gwind_start_flag = W_FALSE;//wind-os开始启动线程调度的标记w_stack_t **gwind_high_stack;//高优先级线程栈指针
w_stack_t **gwind_high_stack;//高优先级线程栈指针w_stack_t **gwind_cur_stack;//当前线程栈指针
w_stack_t **gwind_cur_stack;//当前线程栈指针
void _wind_corevar_init(void)
{
    g_core.irq_nest = 0;
    g_core.switch_nest = 0;
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.ticks_cnt = 0;
    g_core.ms_cnt = 0;
    g_core.sec_count = 0;
    g_core.sreg_idx = 0;
}

static w_bool_t is_switch_enable(void)
{
    return g_core.switch_nest>0?W_FALSE:W_TRUE;
}

void wind_disable_switch(void)
{
    wind_disable_interrupt();
    g_core.switch_nest ++;
    wind_enable_interrupt();
}

void wind_enable_switch(void)
{
    wind_disable_interrupt();
    if(g_core.switch_nest > 0)
        g_core.switch_nest --;
    wind_enable_interrupt();
        _wind_thread_dispatch();
}

extern w_sreg_t  wind_save_sr(void);
extern void   wind_restore_sr(w_sreg_t cpu_sr);

void wind_disable_interrupt(void)
{
    w_sreg_t cpu_sr;
    cpu_sr = wind_save_sr();
    g_core.ssr[g_core.sreg_idx++] = cpu_sr;
    if(g_core.sreg_idx >= 32)
    {
        wind_system_reset();
    }
}

void wind_enable_interrupt(void)
{
    w_sreg_t cpu_sr;
    if(g_core.sreg_idx > 0)
        g_core.sreg_idx --;
    cpu_sr = g_core.ssr[g_core.sreg_idx];   
    wind_restore_sr(cpu_sr);
}

void wind_enter_irq(void)
{
    if(RUN_FLAG == W_FALSE)
    {
        wind_error("enter not rd.");
        return;
    }
    if(g_core.irq_nest < 255)
        g_core.irq_nest ++;
    return;
}



static w_thread_s *wind_search_highthread(void)
{
    w_dnode_s *dnode;
    w_thread_s *thread = W_NULL;
    w_dlist_s *threadlist;
    threadlist = _wind_thread_list();
    wind_disable_interrupt();
    if(g_core.switch_nest > 0)
    {
        thread = wind_thread_current();
        thread->run_times ++;
        wind_enable_interrupt();
        return thread;
    }
    foreach_node(dnode,threadlist)
    {
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,validnode);
        if(thread->runstat == THREAD_STATUS_READY)
        {
            thread->run_times ++;
            wind_enable_interrupt();
            return thread;
        }
    }
    wind_enable_interrupt();
    wind_critical("core NOT find valid thread!");
    return W_NULL;
}

void wind_exit_irq(void)
{
    w_thread_s *thread;
    if(RUN_FLAG == W_FALSE)
    {
        wind_error("exit not rd %d",RUN_FLAG);
        return;
    }
    wind_disable_interrupt();
    if(g_core.irq_nest > 0)
        g_core.irq_nest --;
    
    if((g_core.irq_nest <= 0) && is_switch_enable())
    {
        thread = wind_search_highthread();
        gwind_high_stack = &thread->stack;
        if(gwind_high_stack != gwind_cur_stack)
        {
            wind_interrupt_switch();
        }
    }
    wind_enable_interrupt();
}


//系统调度开始启动运行
static void wind_run()
{
    w_thread_s *thread;
    thread = wind_search_highthread();
    wind_notice("wind multple thread mode.");
    gwind_high_stack = &thread->stack;
    gwind_cur_stack = gwind_high_stack;
    wind_start_switch();
}


//在线程中切换到高优先级的线程
#if WIND_REALTIME_CORE_SUPPORT
void _wind_thread_dispatch(void)
{
    w_thread_s *thread;
    if(RUN_FLAG == W_FALSE)
        return;
    wind_disable_interrupt();
    if(!is_switch_enable())
    {
        wind_enable_interrupt();
        return;
    }
    thread = wind_search_highthread();
    if(thread == wind_thread_current())
    {
        wind_enable_interrupt();
        return;
    }
    gwind_high_stack = &thread->stack;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_enable_interrupt();
        wind_thread_switch();
    }
    else
        wind_enable_interrupt();
}
#else
void _wind_thread_dispatch(void)
{}
#endif


void _wind_switchto_thread(w_thread_s *thread)
{
    wind_disable_interrupt();
    if(thread == wind_thread_current())
    {
        wind_enable_interrupt();
        return;
    }
    gwind_high_stack = &thread->stack;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_enable_interrupt();
        wind_thread_switch();
    }
    wind_enable_interrupt();
}



//获取tick计数器
w_uint32_t wind_get_tick(void)
{
    return g_core.ms_cnt;
}

w_uint32_t wind_get_seconds(void)
{
    return g_core.sec_count;
}

//tick中断调用的函数
void wind_tick_callback(void)
{
    g_core.ticks_cnt ++;//更新tick计数器
    g_core.ms_cnt ++;
    if(g_core.ms_cnt % WIND_TICK_PER_SEC == 0)
        g_core.sec_count ++;
#if WIND_DATETIME_SUPPORT
    wind_time_tick_isr();
#endif
    _wind_thread_wakeup();
}

void wind_tick_isr(void)
{				   
    wind_enter_irq();
    wind_tick_callback();
    wind_exit_irq();       
}


//操作系统初始化
static void _wind_init()
{
    _wind_corevar_init();
    wind_std_init();//调试端口初始化
    wind_os_print_logo();
    _wind_print_sysinfo();
    _wind_thread_mod_init();
    _wind_pool_mod_init();

#if WIND_MUTEX_SUPPORT
    _wind_mutex_mod_init();
#endif
#if WIND_SEM_SUPPORT
    _wind_sem_mod_init();
#endif
#if WIND_MSGBOX_SUPPORT
    _wind_msgbox_mod_init();
#endif
#if WIND_EVENT_SUPPORT
    _wind_event_mod_init();
#endif
#if WIND_PIPE_SUPPORT
    _wind_pipe_mod_init();
#endif
#if WIND_TIMER_SUPPORT
    _wind_timer_mod_init();
#endif
#if WIND_SOFTIRQ_SUPPORT
    _wind_softirq_mod_init();
#endif
#if WIND_DAEMON_SUPPORT
    _wind_daemon_mod_init();
#endif


}



//****************************wind_entry***********************************************

int wind_os_lunch(void)
{
    wind_disable_interrupt();
    _wind_init();
    _create_thread_init();
    wind_enable_interrupt();
    wind_run();
    return W_ERR_OK;
}



