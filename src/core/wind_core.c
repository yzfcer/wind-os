/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_core.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的核心代码
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
#include "wind_core.h"
#include "wind_softint.h"
#include "wind_var.h"
#include "wind_heap.h"
#include "wind_time.h"
#include "core_obj.h"

volatile w_int8_t gwind_int_cnt = 0;//全局的中断计数值
volatile w_int8_t gwind_core_cnt = 0;//全局的禁止切换计数值

extern void wind_thread_switch(void);
extern void wind_interrupt_switch(void);
extern void wind_start_switch(void);


//允许创建用户线程
static void wind_thread_open()
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

void wind_enter_int(void)
{
    if(RUN_FLAG == B_FALSE)
    {
        WIND_ERROR("enter not rd\r\n");
        return;
    }
    if(gwind_int_cnt < 255)
        gwind_int_cnt ++;
    return;
}

static w_bool_t is_switch_enable(void)
{
    return gwind_core_cnt>0?B_FALSE:B_TRUE;
}

static pthread_s wind_search_highthread(void)
{
    pdnode_s pnode;
    pthread_s pthread = NULL;
    wind_close_interrupt();
    if(gwind_core_cnt > 0)
    {
        pthread = wind_thread_current();
        wind_open_interrupt();
        return pthread;
    }
    foreach_node(pnode,&g_core.threadlist)
    {
        pthread = PRI_DLIST_OBJ(pnode,thread_s,validthr);
        if((pthread->used) && (pthread->runstat == THREAD_STATUS_READY))
        {
            wind_open_interrupt();
            return pthread;
        }
    }
    wind_open_interrupt();
    WIND_ERROR("core NOT find valid thread!");
    return NULL;
}
void wind_exit_int(void)
{
    pthread_s pthr;
    if(RUN_FLAG == B_FALSE)
    {
        WIND_ERROR("exit not rd %d\r\n",RUN_FLAG);
        return;
    }
    
    wind_close_interrupt();
    if(gwind_int_cnt > 0)
        gwind_int_cnt --;
    if((gwind_int_cnt == 0) && is_switch_enable())
    {
        pthr = wind_search_highthread();
        gwind_high_stack = &pthr->stack;
        if(gwind_high_stack != gwind_cur_stack)
        {
            wind_interrupt_switch();
        }
    }
    wind_open_interrupt();
}


//系统调度开始启动运行
static void wind_run()
{
    pthread_s pthr;
    pthr = wind_search_highthread();
    gwind_high_stack = &pthr->stack;
    gwind_cur_stack = gwind_high_stack;
    wind_start_switch();
}




//在线程中切换到高优先级的线程
#if WIND_REALTIME_CORE_SUPPORT
void wind_thread_dispatch(void)
{
    pthread_s pthr;
    if(RUN_FLAG == B_FALSE)
        return;
    wind_close_interrupt();
    if(!is_switch_enable())
    {
        wind_open_interrupt();
        return;
    }
    pthr = wind_search_highthread();
    if(pthr == wind_thread_current())
    {
        wind_open_interrupt();
        return;
    }
    gwind_high_stack = &pthr->stack;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_open_interrupt();
        wind_thread_switch();
    }
    wind_open_interrupt();
}
#endif





//操作系统初始化
void wind_init()
{
    g_core.usrthren = B_FALSE;
    wind_target_init();//目标机运行环境初始化
    wind_std_port_init();//调试端口初始化
    wind_print_os_info();
    
#if WIND_STAT_SUPPORT
    wind_stat_init();
#endif
    wind_corepool_init();
    wind_corevar_init();
#if WIND_HEAP_SUPPORT
    wind_heaps_init();
#endif
    wind_time_init();//时间初始化
#if WIND_RTC_SUPPORT
    wind_datetime_init();
#endif
    WIND_INFO("initialization completed!\r\n");
}



//****************************wind_entry***********************************************
int create_init_thread(void);

int wind_os_lunch(void)
{
    wind_init();
    create_init_thread();
    wind_thread_open();
    WIND_INFO("wind is ready for running!\r\n");
    wind_thread_open();
    wind_run();
    return ERR_OK;
}


//C程序的入口
//int main(int argc,char **argv)
int main(void)
{
    wind_os_lunch();
    return 0;
}


