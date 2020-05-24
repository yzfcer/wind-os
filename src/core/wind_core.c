/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_core.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind os的核心启动和线程调度功能
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_config.h"
#include "wind_board_port.h"

#include "wind_sysinfo.h"
#include "wind_thread.h"
#include "wind_coroutine.h"
#include "wind_mutex.h"
#include "wind_sem.h"
#include "wind_event.h"
#include "wind_heap.h"
#include "wind_pool.h"
#include "wind_msgbox.h"
#include "wind_pipe.h"
#include "wind_timer.h"
#include "wind_softirq.h"
#include "wind_daemon.h"
#include "wind_diagnose.h"
#include "wind_fsm.h"

#include "wind_core.h"
#include "wind_time.h"
#include "wind_std.h"
#include "wind_debug.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


extern void wind_thread_switch(void);
extern void wind_interrupt_switch(void);
extern void wind_start_switch(void);
extern int _create_thread_init(void);
extern w_irqreg_t  wind_save_sr(void);
extern void   wind_restore_sr(w_irqreg_t cpu_sr);

//Basic global parameters of wind-os and chain headers of various kernel resources
w_core_var_s g_core;

//Flag for wind-os to start thread scheduling
volatile w_bool_t gwind_start_flag = W_FALSE;

//High priority thread stack pointer
w_stack_t **gwind_high_stack;

//Current thread stack pointer
w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void)
{
    g_core.irq_nest = 0;
    g_core.switch_nest = 0;
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.ticks_cnt = 0;
    g_core.ms_cnt = 0;
    g_core.sec_count = 0;
    g_core.irq_mask_idx = 0;
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


void wind_disable_interrupt(void)
{
    w_irqreg_t cpu_sr;
    if(g_core.irq_mask_idx < IRQ_NEST_DEPTH - 1)
    {
		cpu_sr = wind_save_sr();
		g_core.irq_mask[g_core.irq_mask_idx] = cpu_sr;
		g_core.irq_mask_idx ++;
    }
	else
	{
		WIND_TRAP();
        wind_system_reset();
	}
}

void wind_enable_interrupt(void)
{
    w_irqreg_t cpu_sr;
    if(g_core.irq_mask_idx > 0)
	{
		g_core.irq_mask_idx --;
		cpu_sr = g_core.irq_mask[g_core.irq_mask_idx];
		wind_restore_sr(cpu_sr);
	}
	else
	{
        WIND_TRAP();
    }
}

void wind_enter_irq(void)
{
    if(gwind_start_flag == W_FALSE)
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
        thread = PRIDNODE_TO_THREAD(dnode,validnode);
        if(thread->runstat == THREAD_STATUS_READY)
        {
            thread->run_times ++;
            wind_enable_interrupt();
            return thread;
        }
    }
    wind_enable_interrupt();
    //wind_critical("core NOT find valid thread!");
    return W_NULL;
}

void wind_exit_irq(void)
{
    w_thread_s *thread;
    if(gwind_start_flag == W_FALSE)
    {
        wind_error("exit not rd %d",gwind_start_flag);
        return;
    }
    wind_disable_interrupt();
    if(g_core.irq_nest > 0)
        g_core.irq_nest --;
    
    if((g_core.irq_nest <= 0) && is_switch_enable())
    {
        thread = wind_search_highthread();
        gwind_high_stack = &thread->stack_cur;
        if(gwind_high_stack != gwind_cur_stack)
        {
			wind_enable_interrupt();
            wind_interrupt_switch();
        }
		else
			wind_enable_interrupt();
    }
	else
	    wind_enable_interrupt();
}



static void wind_enter_thread_mode()
{
    w_thread_s *thread;
    thread = wind_search_highthread();
    wind_notice("wind multple thread mode.");
    gwind_high_stack = &thread->stack_cur;
    gwind_cur_stack = gwind_high_stack;
    wind_start_switch();
}


void _wind_thread_dispatch(void)
{
#if WIND_REALTIME_CORE_SUPPORT
    w_thread_s *thread;
    if(gwind_start_flag == W_FALSE)
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
    gwind_high_stack = &thread->stack_cur;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_enable_interrupt();
        wind_thread_switch();
    }
    else
        wind_enable_interrupt();
#endif
}


void _wind_switchto_thread(w_thread_s *thread)
{
    wind_disable_interrupt();
    if(thread == wind_thread_current())
    {
        wind_enable_interrupt();
        return;
    }
    gwind_high_stack = &thread->stack_cur;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_enable_interrupt();
        wind_thread_switch();
    }
    else
        wind_enable_interrupt();
}


w_uint32_t wind_get_tick(void)
{
    return g_core.ticks_cnt;
}

w_err_t wind_get_core_var(w_core_var_s *core_var)
{
    WIND_ASSERT_RETURN(core_var != W_NULL,W_ERR_PTR_NULL);
    wind_disable_interrupt();
    wind_memcpy(core_var,&g_core,sizeof(w_core_var_s));
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_uint32_t wind_get_seconds(void)
{
    return g_core.sec_count;
}


void wind_tick_callback(void)
{
    g_core.ticks_cnt ++;
    g_core.ms_cnt ++;
    if(g_core.ms_cnt % WIND_TICK_PER_SEC == 0)
        g_core.sec_count ++;
#if WIND_DATETIME_SUPPORT
    _wind_datetime_tick_isr();
#endif
    _wind_thread_wakeup();
}

void wind_tick_isr(void)
{				   
    wind_enter_irq();
    wind_tick_callback();
    wind_exit_irq();       
}



static void _wind_init()
{
	wind_init_hook();
    _wind_std_init();
    _wind_os_print_logo();
    _wind_print_sysinfo();
    _wind_pool_mod_init();
#if WIND_DIAGNOSE_SUPPORT
    _wind_diagnose_mod_init();
#endif
    _wind_thread_mod_init();
#if WIND_COROUTINE_SUPPORT
    _wind_coroutine_mod_init();
#endif

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
#if WIND_FSM_SUPPORT
    _wind_fsm_mod_init();
#endif
#if WIND_SOFTIRQ_SUPPORT
    _wind_softirq_mod_init();
#endif
#if WIND_DAEMON_SUPPORT
    _wind_daemon_mod_init();
#endif


}



//****************************wind-os launch entry***********************************************
int wind_os_launch(void)
{
    _wind_corevar_init();
    wind_disable_interrupt();
    _wind_init();
    _create_thread_init();
    wind_enable_interrupt();
    wind_enter_thread_mode();
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

