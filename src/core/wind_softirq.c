/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_softirq.h / wind_softirq.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 用于实现系统的软中断的功能，软中断主要是提供一种在高优先级运行的任务模式，该任务具有所有
**              线程的最高优先级，在执行任务时，除非自己放弃，否则不会被任何其他的线程打断在该模块执行
**              执行期间将不会被其他的任务所中断，但却可以被硬件中断所打断的该优先级模式
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
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
#include "wind_os_hwif.h"
#if WIND_SOFTIRQ_SUPPORT
#define WIND_SOFTINT_STK_LEN 256 //软中断线程的堆栈深度
//软中断线程的堆栈
static w_stack_t softirq_stk[WIND_SOFTINT_STK_LEN];

static w_uint16_t softirq_index = 0;
static thread_s *softirq_thread = NULL;

//static w_handle_t softirq_handle = -1;
softirq_fn wind_soft_vectors[WIND_SOFTINT_MAX_NUM];
//初始化软中断的一些相关参数
void _wind_softirq_init(void)
{
    wind_memset(softirq_stk,0,WIND_SOFTINT_STK_LEN *sizeof(w_stack_t));
}


//向软中断模块注册一个中断向量响应函数
w_int32_t wind_softirq_reg(softirq_fn func)
{
    w_int32_t i;
    w_int32_t irq_id = -1;
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
    {
        if(wind_soft_vectors[i] == func)
        {
            return i;
        }
    }
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
    {
        if(wind_soft_vectors[i] == NULL)
        {
            irq_id = i;
            wind_soft_vectors[i] = func;
            break;
        }
    }
    return irq_id;
}

//取消一个软中断的注册
w_err_t wind_softirq_unreg(w_int32_t irq_id)
{
    if(irq_id < 0 || irq_id >= WIND_SOFTINT_MAX_NUM)
        return ERR_PARAM_OVERFLOW;
    wind_soft_vectors[irq_id] = NULL;
    return ERR_OK;
}

//触发一个软件中断
void wind_softirq_trig(w_int32_t irq_id)
{
    wind_disable_interrupt();
    softirq_index = irq_id;
    softirq_thread->runstat = THREAD_STATUS_READY;
    wind_enable_interrupt();
    _wind_switchto_thread(softirq_thread);
}

static w_err_t wind_softirq_thread(w_int32_t argc,w_int8_t **argv)
{
    w_int32_t i;
    
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
        wind_soft_vectors[i] = NULL;
    while(1)
    {
        wind_disable_interrupt();
        softirq_thread->runstat = THREAD_STATUS_SUSPEND;
        softirq_thread->cause = CAUSE_COM;
        wind_enable_interrupt();
        _wind_thread_dispatch();
        if(wind_soft_vectors[softirq_index] != NULL)
        {
            (wind_soft_vectors[softirq_index])();
        }
    }
    //return ERR_OK;
}

//创建软件中断线程
w_err_t _wind_create_softirq_thread(void)
{
    wind_notice("create soft interrupt thread.");
    softirq_thread = wind_thread_create("softirq",PRIO_HIGH,wind_softirq_thread,
                0,NULL,softirq_stk,WIND_SOFTINT_STK_LEN);
    WIND_ASSERT_RETURN(softirq_thread != NULL,ERR_COMMAN);
    wind_thread_set_priority(softirq_thread,1);
    return ERR_OK;
}



#endif

