/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_softirq.h / wind_softirq.c
** Author      : Jason Zhou
** Last Date   : 
** Description : It is used to realize the function of soft interrupt of the system. Soft interrupt mainly 
**               provides a task mode that runs at high priority. During the execution of the module, it 
**               will not be interrupted by other tasks, but it can be interrupted by hardware interrupt
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_softirq.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_time.h"
#include "wind_core.h"
#include "wind_board_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_SOFTIRQ_SUPPORT
#define SOFT_FLAG_ARR_CNT ((WIND_SOFTINT_MAX_NUM + 31) >> 5)

static w_stack_t softirq_stk[THREAD_SOFTINT_STKSIZE];
static w_thread_s *softirq_thread = W_NULL;
w_softirq_fn softirq_vectors[WIND_SOFTINT_MAX_NUM];
w_uint32_t softirq_flag[SOFT_FLAG_ARR_CNT];


w_err_t _wind_softirq_mod_init(void)
{
    wind_memset(softirq_flag,0,sizeof(softirq_flag));
    wind_memset(softirq_vectors,0,sizeof(softirq_vectors));
    wind_memset(softirq_stk,0,THREAD_SOFTINT_STKSIZE * sizeof(w_stack_t));
    return W_ERR_OK;
}



w_err_t wind_softirq_reg(w_uint16_t irqid,w_softirq_fn func)
{
    wind_notice("register softirq %d",irqid);
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    softirq_vectors[irqid] = func;
    return W_ERR_OK;
}


w_err_t wind_softirq_unreg(w_int32_t irqid)
{
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
	wind_notice("unregister softirq %d",irqid);
    softirq_vectors[irqid] = W_NULL;
    return W_ERR_OK;
}


w_err_t wind_softirq_trig(w_int32_t irqid)
{
    w_thread_s *thread;
    w_int32_t idx1,idx2;
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    WIND_ASSERT_RETURN(softirq_thread != W_NULL,W_ERR_FAIL);
    thread = softirq_thread;
    wind_disable_interrupt();
    
    idx1 = (irqid >> 5);
    idx2 = (irqid & 0x1f);
    softirq_flag[idx1] |= (1 << idx2);
	//wind_printf("g_core.sreg_idx=%d\r\n",g_core.sreg_idx);
    wind_thread_resume(thread);
    wind_enable_interrupt();
    _wind_switchto_thread(thread);
    return W_ERR_OK;
}

static w_softirq_fn get_irq_handle(void)
{
    w_int32_t i,j;
    w_int32_t idx1,idx2;
    
    for(i = 0;i < SOFT_FLAG_ARR_CNT;i ++)
    {
        if(softirq_flag[i] != 0)
        {
            idx1 = i;
            for(j = 0;j < 32;j ++)
            {
                if(softirq_flag[i] & (1 << j))
                {
                    idx2 = j;
                    softirq_flag[idx1] &= (~(0x01 << idx2));
                    return softirq_vectors[(idx1 << 5) + idx2];
                }
            }
        }
    }
    return W_NULL;
}

static w_err_t thread_softirq(w_int32_t argc,char **argv)
{
    w_thread_s *thread;
    w_softirq_fn func;
    thread = wind_thread_current();
    thread->cause = CAUSE_COMMON;
    thread->runstat = THREAD_STATUS_SUSPEND;
    while(W_TRUE)
    {
        _wind_thread_dispatch();
        while(W_TRUE)
        {
            wind_disable_interrupt();
            func = get_irq_handle();
            wind_enable_interrupt();
            if(func == W_NULL)
            {
                thread->cause = CAUSE_COMMON;
                thread->runstat = THREAD_STATUS_SUSPEND;
                break;
            }
            if(func != W_NULL)
                func();
        }
    }
    return W_ERR_OK;
}


w_err_t _wind_create_thread_softirq(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("softirq",thread_softirq,
                0,W_NULL,softirq_stk,THREAD_SOFTINT_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread,F_THREAD_NO_KILL | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,1);
    softirq_thread = thread;
    return W_ERR_OK;
}



#endif // #if WIND_SOFTIRQ_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
