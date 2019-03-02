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
#include "wind_type.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_dlist.h"
#include "wind_core.h"
#include "wind_string.h"
#include "windows.h"
//#include "wind_os_switch.h"
#define GET_OBJ(ptr,type,mbrnode) (void*)(((char*)(ptr))-((w_uint32_t)&(((type*)0)->mbrnode)))

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
        //������ջ
        push ebp
        mov ebp, esp

        //���浱ǰ�̼߳Ĵ���
        push edi
        push esi
        push ebx
        push ecx
        push edx
        push eax

        mov esi, srcthr
        mov edi, destthr

        mov [esi + w_thread_s.stack], esp

        //�����߳��л�������һ���̸߳���
        mov esp, [edi + w_thread_s.stack]

        pop eax
        pop edx
        pop ecx
        pop ebx
        pop esi
        pop edi

        pop ebp
        ret    //��startup(�̺߳������)����eip ִ�еľ����̺߳�����
    }
}




w_sreg_t wind_save_sr(void)
{

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
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,sleepnode);
        if(wind_strcmp(thread->name,name) == 0)
            return thread;
    }
    return W_NULL;
}

static void set_sleep(w_thread_s *thread,w_uint32_t ms)
{
    w_uint16_t stcnt;
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
    gwind_cur_stack = &mainthr.stack;
    switch_context(cur,high);
    while(1)
    {
        Sleep(1000/WIND_TICK_PER_SEC);
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
    cur = GET_OBJ(gwind_cur_stack,w_thread_s,stack);
    high = GET_OBJ(gwind_high_stack,w_thread_s,stack);
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



