/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_core.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ����������̵߳��ȹ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
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


w_core_var_s g_core;//wind-os�Ļ���ȫ�ֲ����͸����ں���Դ������ͷ
volatile w_bool_t gwind_start_flag = W_FALSE;//wind-os��ʼ�����̵߳��ȵı��w_stack_t **gwind_high_stack;//�����ȼ��߳�ջָ��
w_stack_t **gwind_high_stack;//�����ȼ��߳�ջָ��w_stack_t **gwind_cur_stack;//��ǰ�߳�ջָ��
w_stack_t **gwind_cur_stack;//��ǰ�߳�ջָ��
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


//ϵͳ���ȿ�ʼ��������
static void wind_run()
{
    w_thread_s *thread;
    thread = wind_search_highthread();
    wind_notice("wind multple thread mode.");
    gwind_high_stack = &thread->stack;
    gwind_cur_stack = gwind_high_stack;
    wind_start_switch();
}


//���߳����л��������ȼ����߳�
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



//��ȡtick������
w_uint32_t wind_get_tick(void)
{
    return g_core.ms_cnt;
}

w_uint32_t wind_get_seconds(void)
{
    return g_core.sec_count;
}

//tick�жϵ��õĺ���
void wind_tick_callback(void)
{
    g_core.ticks_cnt ++;//����tick������
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


//����ϵͳ��ʼ��
static void _wind_init()
{
    _wind_corevar_init();
    wind_std_init();//���Զ˿ڳ�ʼ��
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



