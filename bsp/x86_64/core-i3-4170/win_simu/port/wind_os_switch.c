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
#include "windows.h"
#include "time.h"
//#include  < MMSystem.h >
//#pragma comment(lib, "winmm.lib")

//#include "wind_os_switch.h"
#define GET_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_addr_t)&(((type*)0)->mbrnode)))

extern volatile w_bool_t gwind_start_flag;
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;
extern void wind_tick_isr(void);
w_thread_s *curthread = W_NULL;
w_thread_s mainthr;
w_stack_t mainstk[2048];
void wind_interrupt_switch(void);

__declspec(naked) static void switch_context(w_thread_s* srcthr, w_thread_s* destthr)
{
    __asm 
    {
        //提升堆栈
        push ebp
        mov ebp, esp

        //保存当前线程寄存器
        push edi
        push esi
        push ebx
        push ecx
        push edx
        push eax

        //经典线程切换，另外一个线程复活
        mov esi, srcthr
        mov edi, destthr
        mov [esi + w_thread_s.stack_cur], esp
        mov esp, [edi + w_thread_s.stack_cur]
		//mov [esi + 48], esp
		//mov esp, [edi + 48]

        pop eax
        pop edx
        pop ecx
        pop ebx
        pop esi
        pop edi

        pop ebp
        ret    //把startup(线程函数入口)弹到eip 执行的就是线程函数了
    }
}




w_sreg_t wind_save_sr(void)
{
    return 0;
}

void wind_restore_sr(w_sreg_t sreg)
{

}
w_thread_s *get_sleep_thread(char *name)
{
    w_thread_s *thread;
    w_dnode_s *dnode;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    if((sleeplist == W_NULL) || (sleeplist->head == W_NULL))
        return W_NULL;
    foreach_node(dnode,sleeplist)
    {
        thread = PRIDNODE_TO_THREAD(dnode,sleepnode);
        if(wind_strcmp(thread->name,name) == 0)
            return thread;
    }
    return W_NULL;
}

static void set_sleep(w_thread_s *thread,w_int32_t ms)
{
    w_int32_t stcnt;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    stcnt = ms *WIND_TICK_PER_SEC / 1000;
    if(0 == stcnt)
        stcnt = 1;
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_SLEEP;
    thread->sleep_ticks = stcnt;
    if(get_sleep_thread("main") == W_NULL)
        dlist_insert_prio(sleeplist,&thread->sleepnode,thread->prio);
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
        Sleep(1000/WIND_TICK_PER_SEC);
        //timeEndPeriod(1);
        set_sleep(&mainthr,1000000);
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



