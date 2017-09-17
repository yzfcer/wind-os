/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_var.h / wind_var.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的内核相关的变量的集合
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.11.27
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_list.h"
#include "wind_pipe.h"
#include "wind_message.h"
#include "wind_sem.h"
#include "wind_lock.h"
#include "wind_timer.h"
#include "wind_err.h"
#include "wind_mpool.h"
#include "wind_var.h"
#include "wind_assert.h"
core_var_s g_core;
volatile w_bool_t gwind_start_flag = B_FALSE;
pthread_s gwind_cur_pcb = NULL;//当前线程PCB指针
pthread_s gwind_high_pcb = NULL;//最高优先级PCB指针

void wind_corepool_init(void)
{
    wind_mpool_create("pcb_pool",g_core.pcb,sizeof(g_core.pcb),sizeof(thread_s));
    wind_mpool_create("node_pool",g_core.node,sizeof(g_core.node),sizeof(node_s));
#if WIND_PIPE_SUPPORT > 0
    wind_mpool_create("pipe_pool",g_core.pipe,sizeof(g_core.pipe),sizeof(pipe_s));
    
#endif
#if WIND_MESSAGE_SUPPORT > 0
    wind_mpool_create("msg_pool",g_core.msg,sizeof(g_core.msg),sizeof(msg_s));
    wind_mpool_create("mbox_pool",g_core.mbox,sizeof(g_core.mbox),sizeof(mbox_s));
#endif
#if WIND_SEM_SUPPORT > 0
    wind_mpool_create("sem_pool",g_core.sem,sizeof(g_core.sem),sizeof(sem_s));
#endif
#if WIND_TIMER_SUPPORT > 0
    wind_mpool_create("ttimer_pool",g_core.ttimer,sizeof(g_core.ttimer),sizeof(ttimer_s));
#endif
#if WIND_LOCK_SUPPORT > 0
    wind_mpool_create("lock_pool",g_core.lock,sizeof(g_core.lock),sizeof(lock_s));
#endif
    wind_mpool_create("stkbuf_pool",g_core.stkbuf,sizeof(g_core.stkbuf),2048 * sizeof(w_stack_t));
}
void print_core_pool(void)
{
    
}

void wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.pcbcnt = 0;
    g_core.usrprocen = B_FALSE;
    g_core.run_falg = B_FALSE;
    g_core.ticks_cnt = 0;
    wind_corepool_init();
    wind_list_init(&g_core.threadlist);
    wind_list_init(&g_core.semlist);
    wind_list_init(&g_core.ttmerlist);
}

void *wind_core_alloc(stat_e type)
{
    void *p;
    switch(type)
    {
    case STAT_PROC:
        p = wind_mpool_alloc(g_core.pcb);
        break;
    case STAT_NODE:
        p = wind_mpool_alloc(g_core.node);
        break;
#if WIND_LOCK_SUPPORT > 0
    case STAT_LOCK:
        p = wind_mpool_alloc(g_core.lock);
        break;
#endif
#if WIND_SEM_SUPPORT > 0
    case STAT_SEM:
        p = wind_mpool_alloc(g_core.sem);
        break;
#endif
#if WIND_PIPE_SUPPORT > 0
    case STAT_PIPE:
        p = wind_mpool_alloc(g_core.pipe);
        break;
#endif
#if WIND_MESSAGE_SUPPORT > 0
    case STAT_MESSAGE:
        p = wind_mpool_alloc(g_core.msg);
        break;
#endif
#if WIND_TIMER_SUPPORT > 0
    case STAT_TIMER:
        p = wind_mpool_alloc(g_core.ttimer);
        break;
#endif
    default:p = NULL;
    }
    if(p != NULL)
    {
        WIND_STAT_INC(type);
        WIND_STAT_MAX(type);
    }
    return p;
}

w_err_t wind_core_free(stat_e type,void *block)
{
    w_err_t err;
    WIND_ASSERT_RETURN(block != NULL,ERR_NULL_POINTER);
    switch(type)
    {
    case STAT_PROC:
        err = wind_mpool_free(g_core.pcb,block);
        break;
    case STAT_NODE:
        err = wind_mpool_free(g_core.node,block);
        break;
#if WIND_LOCK_SUPPORT > 0
    case STAT_LOCK:
        err = wind_mpool_free(g_core.lock,block);
        break;
#endif
#if WIND_SEM_SUPPORT > 0
    case STAT_SEM:
        err = wind_mpool_free(g_core.sem,block);
        break;
#endif
#if WIND_PIPE_SUPPORT > 0
    case STAT_PIPE:
        err = wind_mpool_free(g_core.pipe,block);
        break;
#endif
#if WIND_MESSAGE_SUPPORT > 0
    case STAT_MESSAGE:
        err = wind_mpool_free(g_core.msg,block);
        break;
#endif
#if WIND_TIMER_SUPPORT > 0
    case STAT_TIMER:
        err = wind_mpool_free(g_core.ttimer,block);
        break;
#endif
    default:
        err = ERR_COMMAN;
        break;
    }
    if(err == ERR_OK)
    {
        WIND_STAT_MINUS(type);
    }
    return err;
}

//申请一个线程堆栈
pstack_t wind_stack_alloc(w_uint32_t size)
{
    w_uint16_t stksize;
    pstack_t pstk;
    if(size == 0)
        return NULL;
    if(stksize <= 16)
    {
        pstk = wind_mpool_alloc(g_core.stkbuf);
        if(pstk)
            return pstk;
    }
    return NULL;
}

//释放一个线程堆栈
w_err_t wind_stack_free(pstack_t pstack)
{
    WIND_ASSERT_RETURN(pstack != NULL,ERR_NULL_POINTER);
        return wind_mpool_free(g_core.stkbuf,pstack);
}
