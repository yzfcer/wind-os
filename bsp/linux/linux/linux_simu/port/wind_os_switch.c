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
#include <ucontext.h>

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




ucontext_t mainctx;
void wind_start_switch(void)
{
	ucontext_t ctx;
	ucontext_t *saved;
    w_thread_s *cur,*high;
	w_stack_t *stk;
	w_int32_t ctx_size;
    gwind_start_flag = W_TRUE;
    IDLE_CNT_PER_SEC = 1000000;
	
	getcontext(&ctx);
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
	
	
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(high->stack_start + high->stksize - 1 - ctx_size);
	saved = (ucontext_t *)stk;
	wind_memcpy(&ctx, stk, sizeof(ucontext_t));
	wind_printf("high thread:%s\n",high->name);
	wind_printf("high stksize:0x%x\n",high->stksize);
    //ctx.uc_link = saved->uc_link; /// next context   
    //ctx.uc_stack.ss_sp = saved->uc_stack.ss_sp; /// base address    
    //ctx.uc_stack.ss_size = saved->uc_stack.ss_size;
    //ctx.uc_stack.ss_flags = saved->uc_stack.ss_flags;
	
	//wind_printf("stack_start:%p\n",high->stack_start);
	//wind_printf("stk:%p\n",stk);
	wind_printf("ctx.uc_stack.ss_sp=%p\n",ctx.uc_stack.ss_sp);
    wind_printf("ctx.uc_stack.ss_size=0x%x\n",ctx.uc_stack.ss_size);

	swapcontext(&mainctx,&ctx);
	WIND_TRAP();
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



