/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_os_switch.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.02.27
**��        ��: wind-os���߳��������л���غ���
**��        ��: 
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.02.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.02.27
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_dlist.h"
#include "wind_core.h"
#include "wind_string.h"
#include <unistd.h> 
#include <signal.h> 
#include <ucontext.h>
#include <sys/time.h>
#include <errno.h>

#define GET_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbrnode)))

extern volatile w_bool_t gwind_start_flag;
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;
extern void wind_tick_isr(void);
w_thread_s *curthread = W_NULL;
w_thread_s mainthr;
w_stack_t mainstk[2048];
void wind_interrupt_switch(void);
ucontext_t mainctx;

static void switch_context(w_thread_s* srcthr, w_thread_s* destthr)
{
	w_stack_t *stk;
	w_int32_t ctx_size;
	ucontext_t *srcctx,*destctx;
    //wind_printf("switch_context:%s->%s\n",srcthr->name,destthr?destthr->name:"null");
	WIND_ASSERT_RETURN_VOID(srcthr != W_NULL);
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(srcthr->stack_start + srcthr->stksize - 1 - ctx_size);
	srcctx = (ucontext_t *)stk;
	//if(destthr == W_NULL)
	//	WIND_TRAP();

	if(destthr != W_NULL)
	{
		ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
		stk = (w_stack_t*)(destthr->stack_start + destthr->stksize - 1 - ctx_size);
		destctx = (ucontext_t *)stk;
		//destctx->uc_link = (ucontext_t *)stk;
		destctx->uc_link = NULL;
	}
	else
	{
		//wind_printf("switch_context to main\n");
		destctx = &mainctx;
		//destctx->uc_link = &mainctx;
		destctx->uc_link = NULL;
		//return;
	}
	swapcontext(srcctx,destctx);
}


static sigset_t g_sig[32];
static int sig_idx = 0;
w_irqreg_t wind_save_sr(void)
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

void wind_restore_sr(w_irqreg_t sreg)
{
	sig_idx --;
	//printf("sig_idx[%d] unblock:%p\n",sig_idx,&g_sig[sig_idx]);
    sigprocmask(SIG_SETMASK, sreg, NULL);
}

#if 0
static w_err_t main_thread(w_int32_t argc,char **argv)
{

    return W_ERR_OK;
}
#endif

static void sig_ticks_timer(int signo)
{
	//printf("wind_tick_isr\n");
	wind_tick_isr();
}

static void tick_timer_run(void)
{
#if 1
	static int timer_flag = 0;
	struct itimerval tv, otv;
	if(timer_flag == 0)
	{
		timer_flag = 1;
		signal(SIGALRM, sig_ticks_timer);
		tv.it_value.tv_sec = 0;
		tv.it_value.tv_usec = 1000000/WIND_TICK_PER_SEC;
		tv.it_interval.tv_sec = 0;
		tv.it_interval.tv_usec = 1000000/WIND_TICK_PER_SEC;
		wind_printf("tick interval is %d usec\n",tv.it_interval.tv_usec);
		if (setitimer(ITIMER_REAL, &tv, &otv) != 0)
			printf("setitimer err %d\n", errno);	
	}

#endif
	//int cnt = 0;
	while(1) 
	{
		sleep(10);
		//*(char*)0 = 0;
	}
}


void wind_start_switch(void)
{
	static int init_flag = 0;
	ucontext_t *ctx;

    w_thread_s *high;
	w_stack_t *stk;
	w_int32_t ctx_size;
    gwind_start_flag = W_TRUE;
    g_core.idle_cnt_max = 1000000;
	wind_debug("sizeof(sigset_t)=%d\n",sizeof(sigset_t));
	wind_debug("sizeof(w_irqreg_t)=%d\n",sizeof(w_irqreg_t));
	getcontext(&mainctx);

	if(init_flag == 0)
	{
		init_flag = 1;
		high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
		ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
		stk = (w_stack_t*)(high->stack_start + high->stksize - 1 - ctx_size);
		ctx = (ucontext_t *)stk;
		setcontext(ctx);
		//swapcontext(&mainctx,ctx);
	}
	//wind_printf("tick_timer_run\n");
	tick_timer_run();
	
	WIND_TRAP();
}

static w_err_t idle_func(w_int32_t argc,char **argv)
{
	printf("enter idle_func\n");
    while(1)
	{
		printf("idle_func\n");
        wind_thread_sleep(1000);
	}
    return W_ERR_OK;
}

w_err_t do_switch(void)
{
    w_thread_s *cur,*high;
    cur = GET_OBJ(gwind_cur_stack,w_thread_s,stack_cur);
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack_cur);
    gwind_cur_stack = gwind_high_stack;
    //wind_printf("do_switch\n");
    if(high == W_NULL)
 	{
        //high = wind_thread_get("idle");
		//return W_ERR_FAIL;
		//wind_printf("high == W_NULL\n");
	}
	if(cur == W_NULL)
	{
		//wind_printf("cur == W_NULL\n");
        cur = wind_thread_get("idle");
	}
    if((high != W_NULL) && (wind_strcmp(high->name,"idle") == 0))
	{
		//printf("high is idle\n");
        high->thread_func = idle_func;		
	}
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
    while(W_ERR_OK != do_switch())
		printf("wind_thread_switch\n");
}

void wind_interrupt_switch(void)
{
    while(W_ERR_OK != do_switch())
		printf("wind_interrupt_switch\n");
}



