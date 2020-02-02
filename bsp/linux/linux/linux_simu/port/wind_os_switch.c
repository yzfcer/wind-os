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
	w_stack_t *stk;
	w_int32_t ctx_size;
	ucontext_t *srcctx,*destctx;
    wind_printf("switch_context:%s->%s\n",srcthr->name,destthr->name);
	WIND_ASSERT_RETURN_VOID(srcthr != W_NULL);
	WIND_ASSERT_RETURN_VOID(destthr != W_NULL);
	WIND_ASSERT_RETURN_VOID(srcthr != destthr);
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(srcthr->stack_start + srcthr->stksize - 1 - ctx_size);
	srcctx = (ucontext_t *)stk;
	
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(destthr->stack_start + destthr->stksize - 1 - ctx_size);
	destctx = (ucontext_t *)stk;
	swapcontext(srcctx,destctx);
}


static sigset_t g_sig[32];
static int sig_idx = 0;
w_sreg_t wind_save_sr(void)
{
    sigset_t set;
	int idx;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);
	idx = sig_idx;
    sigprocmask(SIG_BLOCK, &set, &g_sig[idx]);
	sig_idx ++;
	//printf("sig_idx[%d] block:%p\n",idx,&g_sig[idx]);
    return &g_sig[idx];
}

void wind_restore_sr(w_sreg_t sreg)
{
	sig_idx --;
    sigprocmask(SIG_SETMASK, sreg, NULL);
	//printf("sig_idx[%d] unblock:%p\n",sig_idx,sreg);
}

void print_sig_nesting(void)
{
	wind_printf("sig_idx=%d\n",sig_idx);
}



static w_err_t main_thread(w_int32_t argc,char **argv)
{

    return W_ERR_OK;
}




void wind_start_switch(void)
{
	ucontext_t mainctx;
	ucontext_t *ctx;
	//ucontext_t *saved;
    w_thread_s *high;
	w_stack_t *stk;
	w_int32_t ctx_size;
    gwind_start_flag = W_TRUE;
    IDLE_CNT_PER_SEC = 1000000;
	wind_printf("sizeof(sigset_t)=%d\n",sizeof(sigset_t));
	wind_printf("sizeof(w_sreg_t)=%d\n",sizeof(w_sreg_t));
	
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(high->stack_start + high->stksize - 1 - ctx_size);
	ctx = (ucontext_t *)stk;
	//getcontext(ctx);
	
	swapcontext(&mainctx,ctx);
	WIND_TRAP();
}

static w_err_t idle_func(w_int32_t argc,char **argv)
{
    while(1)
        wind_thread_sleep(1000);
    return W_ERR_OK;
}
//#include <sys/time.h>
//extern struct itimerval ticks_timer;
w_err_t do_switch(void)
{
    w_thread_s *cur,*high;
    cur = GET_OBJ(gwind_cur_stack,w_thread_s,stack_cur);
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
    gwind_cur_stack = gwind_high_stack;
    //wind_printf("do_switch\n");
	//print_sig_nesting();
    if(high == W_NULL)
 	{
        high = wind_thread_get("idle");
		
		//wind_printf("usleep\n");
		usleep(100000);
		
		return W_ERR_FAIL;
	}
	if(cur == W_NULL)
	{
		//wind_printf("cur == W_NULL\n");
        cur = wind_thread_get("idle");
	}
	
    if(wind_strcmp(high->name,"idle") == 0)
        high->thread_func = idle_func;
    if(high == cur)
	{
		//WIND_TRAP();
		//return;
	}
        
    switch_context(cur,high);
	return W_ERR_OK;
}

void wind_thread_switch(void)
{
    while(W_ERR_OK != do_switch());
}

void wind_interrupt_switch(void)
{
    while(W_ERR_OK != do_switch());
}



