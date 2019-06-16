/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_softirq.h / wind_softirq.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 用于实现系统的软中断的功能，软中断主要是提供一种在高优先级运行的任务模式，该任务具有所有
**              线程的最高优先级，在执行任务时，除非自己放弃，否则不会被任何其他的线程打断在该模块执行
**              执行期间将不会被其他的任务所中断，但却可以被硬件中断所打断的该优先级模式
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 2013.10.23未完待续
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
#if WIND_SOFTIRQ_SUPPORT
#define SOFT_FLAG_ARR_CNT ((WIND_SOFTINT_MAX_NUM + 31) >> 5)
//软中断线程的堆栈
static w_stack_t softirq_stk[THREAD_SOFTINT_STKSIZE];
static w_thread_s *softirq_thread = W_NULL;
w_softirq_fn softirq_vectors[WIND_SOFTINT_MAX_NUM];
w_uint32_t softirq_flag[SOFT_FLAG_ARR_CNT];

//初始化软中断的一些相关参数
w_err_t _wind_softirq_mod_init(void)
{
    wind_memset(softirq_flag,0,sizeof(softirq_flag));
    wind_memset(softirq_vectors,0,sizeof(softirq_vectors));
    wind_memset(softirq_stk,0,THREAD_SOFTINT_STKSIZE * sizeof(w_stack_t));
    return W_ERR_OK;
}


//向软中断模块注册一个中断向量响应函数
w_err_t wind_softirq_reg(w_uint16_t irqid,w_softirq_fn func)
{
    wind_notice("register softirq %d",irqid);
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    softirq_vectors[irqid] = func;
    return W_ERR_OK;
}

//取消一个软中断的注册
w_err_t wind_softirq_unreg(w_int32_t irqid)
{
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
	wind_notice("unregister softirq %d",irqid);
    softirq_vectors[irqid] = W_NULL;
    return W_ERR_OK;
}

//触发一个软件中断
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
            if(func == W_NULL)
            {
                thread->cause = CAUSE_COMMON;
                thread->runstat = THREAD_STATUS_SUSPEND;
                wind_enable_interrupt();
                break;
            }
            wind_enable_interrupt();
            if(func != W_NULL)
                func();
        }
    }
}

//创建软件中断线程
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



#endif

