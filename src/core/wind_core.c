/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_core.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的核心启动和线程调度功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_config.h"
#include "wind_os_hwif.h"

#include "wind_version.h"
#include "wind_thread.h"
#include "wind_mutex.h"
#include "wind_sem.h"
#include "wind_msgbox.h"
#include "wind_pipe.h"
#include "wind_timer.h"
#include "wind_softirq.h"

#include "wind_core.h"
#include "wind_var.h"
#include "wind_time.h"
#include "wind_std.h"
#include "wind_debug.h"

volatile w_int8_t gwind_int_nest = 0;//全局的中断嵌套计数值
volatile w_int8_t gwind_core_cnt = 0;//全局的禁止切换计数值

extern void wind_thread_switch(void);
extern void wind_interrupt_switch(void);
extern void wind_start_switch(void);


//允许创建用户线程
static void _wind_thread_open()
{
    g_core.usrthren = B_TRUE;
}

//查看是否允许创建用户线程
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

//SREG，CPU状态寄存器对应的数据位宽，当关闭中断时需要保存这个寄存器
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
    if(RUN_FLAG == B_FALSE)
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
    return gwind_core_cnt>0?B_FALSE:B_TRUE;
}

static thread_s *wind_search_highthread(void)
{
    dnode_s *dnode;
    thread_s *thread = NULL;
    wind_disable_interrupt();
    if(gwind_core_cnt > 0)
    {
        thread = wind_thread_current();
        wind_enable_interrupt();
        return thread;
    }
    foreach_node(dnode,&g_core.threadlist)
    {
        thread = PRI_DLIST_OBJ(dnode,thread_s,validnode);
        if(thread->runstat == THREAD_STATUS_READY)
        {
            wind_enable_interrupt();
            return thread;
        }
    }
    wind_enable_interrupt();
    wind_error("core NOT find valid thread!");
    return NULL;
}

void wind_exit_irq(void)
{
    thread_s *thread;
    if(RUN_FLAG == B_FALSE)
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


//系统调度开始启动运行
static void wind_run()
{
    thread_s *thread;
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
    thread_s *thread;
    if(RUN_FLAG == B_FALSE)
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


void _wind_switchto_thread(thread_s *thread)
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


//操作系统初始化
static void _wind_init()
{
    g_core.usrthren = B_FALSE;
    _wind_target_init();//目标机运行环境初始化
    _wind_std_init();//调试端口初始化
    
    _wind_print_os_info();
    _wind_corevar_init();
    _wind_thread_init();
    _wind_mutex_init();
    _wind_sem_init();
    _wind_msgbox_init();
    _wind_pipe_init();
    _wind_timer_init();
    _wind_softirq_init();
    
    _wind_tick_init();//时间初始化
}



//****************************wind_entry***********************************************
int _create_init_thread(void);

static int wind_os_lunch(void)
{
    wind_disable_interrupt();
    _wind_init();
    _create_init_thread();
    _wind_thread_open();
    wind_enable_interrupt();
    wind_run();
    return ERR_OK;
}


//C程序的入口
int main(void)
{
    wind_os_lunch();
    return 0;
}


