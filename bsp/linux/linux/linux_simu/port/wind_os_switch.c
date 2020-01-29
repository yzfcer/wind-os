/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_os_switch.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.02.27
**描        述: wind-os的线程上下文切换相关函数
**功        能: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.02.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.02.27
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_dlist.h"
#include "wind_core.h"
#include "wind_string.h"
#include <unistd.h> 
#include <signal.h> 


#define GET_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbrnode)))

extern volatile w_bool_t gwind_start_flag;
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;
extern void wind_tick_isr(void);
w_thread_s *curthread = W_NULL;
w_thread_s mainthr;
w_stack_t mainstk[2048];
void wind_interrupt_switch(void);

static void switch_context(w_thread_s* srcthr, w_thread_s* destthr)
{

}




w_sreg_t wind_save_sr(void)
{
    sigset_t set;
    sigset_t cpu_sr;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);
    sigprocmask(SIG_SETMASK, &set, &cpu_sr);
    return cpu_sr;
}

void wind_restore_sr(w_sreg_t sreg)
{
    sigprocmask(SIG_SETMASK, &sreg, NULL);
}




static w_err_t main_thread(w_int32_t argc,char **argv)
{

    return W_ERR_OK;
}





void wind_start_switch(void)
{
    w_thread_s *cur,*high;
    wind_thread_init(&mainthr,"main",main_thread,0,W_NULL,mainstk,2048);
    gwind_start_flag = W_TRUE;
    IDLE_CNT_PER_SEC = 1000000;
    cur = &mainthr;
    high = &mainthr;//GET_OBJ(gwind_high_stack,w_thread_s,stack);
    gwind_cur_stack = &mainthr.stack_cur;
    switch_context(cur,high);
    
    while(1)
    {
        //timeBeginPeriod(1);
        usleep(1000/WIND_TICK_PER_SEC*1000);
        //timeEndPeriod(1);
        //set_sleep(&mainthr,1000000);
        wind_tick_isr();
    }
}

static w_err_t idle_func(w_int32_t argc,char **argv)
{
    while(1)
        wind_thread_sleep(1000);
    return W_ERR_OK;
}

void do_switch(void)
{
    w_thread_s *cur,*high;
    cur = GET_OBJ(gwind_cur_stack,w_thread_s,stack_cur);
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
    gwind_cur_stack = gwind_high_stack;
    if(high == W_NULL)
        high = &mainthr;
    if(cur == W_NULL)
        cur = &mainthr;
    if(wind_strcmp(high->name,"idle") == 0)
        high->thread_func = idle_func;
    if(high == cur)
        return;
    switch_context(cur,high);
}

void wind_thread_switch(void)
{
    do_switch();
}

void wind_interrupt_switch(void)
{
    do_switch();
}



