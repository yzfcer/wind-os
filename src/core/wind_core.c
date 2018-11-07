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
#include "wind_event.h"

#include "wind_core.h"
#include "wind_time.h"
#include "wind_std.h"
#include "wind_debug.h"

volatile w_int8_t gwind_int_nest = 0;//ȫ�ֵ��ж�Ƕ�׼���ֵ
volatile w_int8_t gwind_core_cnt = 0;//ȫ�ֵĽ�ֹ�л�����ֵ

extern void wind_thread_switch(void);
extern void wind_interrupt_switch(void);
extern void wind_start_switch(void);
extern int _create_thread_init(void);


w_core_var_s g_core;//wind-os�Ļ���ȫ�ֲ����͸����ں���Դ������ͷ
volatile w_bool_t gwind_start_flag = W_FALSE;//wind-os��ʼ�����̵߳��ȵı��
w_stack_t **gwind_high_stack;//�����ȼ��߳�ջָ��
w_stack_t **gwind_cur_stack;//��ǰ�߳�ջָ��

void _wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.usrthren = W_FALSE;
    g_core.ticks_cnt = 0;
}


//�������û��߳�
static void _wind_thread_open()
{
    g_core.usrthren = W_TRUE;
}

//�鿴�Ƿ��������û��߳�
w_bool_t wind_thread_isopen()
{
    return g_core.usrthren;
}

void wind_disable_switch(void)
{
    gwind_core_cnt ++;
}

void wind_enable_switch(void)
{
    if(gwind_core_cnt > 0)
        gwind_core_cnt --;
}

//SREG��CPU״̬�Ĵ�����Ӧ������λ�����ر��ж�ʱ��Ҫ��������Ĵ���
typedef unsigned int sreg_t;
extern sreg_t  wind_save_sr(void);
extern void   wind_restore_sr(sreg_t cpu_sr);
static sreg_t ssr[32];
static w_int32_t sreg_idx = 0;
void wind_disable_interrupt(void)
{
    sreg_t cpu_sr;
    cpu_sr = wind_save_sr();
    ssr[sreg_idx++] = cpu_sr;
    if(sreg_idx >= 32)
    {
        wind_critical("int nest error.");
        wind_system_reset();
    }
}

void wind_enable_interrupt(void)
{
    sreg_t cpu_sr;
    if(sreg_idx > 0)
        sreg_idx --;
    cpu_sr = ssr[sreg_idx];   
    wind_restore_sr(cpu_sr);
}

void wind_enter_irq(void)
{
    if(RUN_FLAG == W_FALSE)
    {
        wind_error("enter not rd.");
        return;
    }
    if(gwind_int_nest < 255)
        gwind_int_nest ++;
    return;
}

static w_bool_t is_switch_enable(void)
{
    return gwind_core_cnt>0?W_FALSE:W_TRUE;
}

static w_thread_s *wind_search_highthread(void)
{
    w_dnode_s *dnode;
    w_thread_s *thread = W_NULL;
    w_dlist_s *threadlist;
    threadlist = _wind_thread_list();
    wind_disable_interrupt();
    if(gwind_core_cnt > 0)
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
    if(gwind_int_nest > 0)
        gwind_int_nest --;
    if((gwind_int_nest == 0) && is_switch_enable())
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
void _wind_thread_dispatch(void){}
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


//����ϵͳ��ʼ��
static void _wind_init()
{
    g_core.usrthren = W_FALSE;
    wind_std_init();//���Զ˿ڳ�ʼ��
    wind_os_print_logo();
    _wind_print_sysinfo();
    _wind_corevar_init();
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

    _wind_tick_init();//ʱ���ʼ��
}



//****************************wind_entry***********************************************

static int wind_os_lunch(void)
	
{
    
    _wind_enter_main_hook();
    wind_disable_interrupt();
    _wind_init();
    _create_thread_init();
    _wind_thread_open();
    wind_enable_interrupt();
    wind_run();
    return W_ERR_OK;
}


//C��������
int main(int argc,char **argv)
{
    wind_os_lunch();
    return 0;
}


