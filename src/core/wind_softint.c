/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_softint.h / wind_softint.c
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
#include "wind_types.h"
#include "wind_softint.h"
#include "wind_err.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_time.h"
#include "wind_assert.h"

#if WIND_SOFTINT_SUPPORT > 0
extern pthread_s wind_get_cur_proc(void);
extern void wind_thread_dispatch(void);
//软中断线程的堆栈
static stack_t softint_stk[WIND_SOFTINT_STK_LEN];
static u16_t softint_index = 0;
static pthread_s softint_ppcb = NULL;
//static HANDLE softint_handle = -1;
softinthandler_f wind_soft_vectors[WIND_SOFTINT_MAX_NUM];
//初始化软中断的一些相关参数
void wind_softint_init(void)
{
    wind_memset(softint_stk,0,WIND_SOFTINT_STK_LEN * sizeof(stack_t));
}
static err_t wind_softint_proc(s16_t argc,s8_t **argv)
{
    err_t err = -1;
    s32_t i;
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
    {
        wind_soft_vectors[i] = NULL;
    }
    while(1)
    {
        wind_close_interrupt();
        softint_ppcb->proc_status = PROC_STATUS_SLEEP;
        softint_ppcb->cause = CAUSE_COM;
        wind_open_interrupt();
        wind_thread_dispatch();
        if(wind_soft_vectors[softint_index] != NULL)
        {
            (wind_soft_vectors[softint_index])();
        }
        
    }
    return ERR_OK;
}

//向软中断模块注册一个中断向量响应函数
HANDLE wind_softint_reg(softinthandler_f func)
{
    HANDLE hint = -1;
    s16_t i;
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
            hint = i;
            wind_soft_vectors[i] = func;
            break;
        }
    }
    return hint;
}

//取消一个软中断的注册
err_t wind_softint_unreg(HANDLE hint)
{
    if(hint < 0 || hint >= WIND_SOFTINT_MAX_NUM)
        return ERR_PARAM_OVERFLOW;
    wind_soft_vectors[hint] = NULL;
    return ERR_OK;
}

//触发一个软件中断
void wind_soft_int(HANDLE handler)
{
    wind_close_interrupt();
    softint_index = handler;
    softint_ppcb->proc_status = PROC_STATUS_READY;
    wind_open_interrupt();
    wind_thread_dispatch();
}

//创建软件中断线程
err_t wind_create_softint_proc(void)
{
    softint_ppcb = wind_thread_create("softint",PRIO_HIGH,wind_softint_proc,
                0,NULL,softint_stk,WIND_SOFTINT_STK_LEN);
                wind_thread_changeprio(softint_ppcb,0);
    //WIND_ERROR("software OK\r\n");
    return ERR_OK;
}


//-----------------------软件中断测试---------------------------------------------
void softint_output(void)
{
    WIND_STD_OUT("wind softint test---OK\r\n");
}

err_t wind_softint_test(void)
{
    HANDLE h;
    h = wind_softint_reg(softint_output);
    wind_soft_int(h);
    //wind_softint_unreg(h);
    return ERR_OK;
}
//-----------------------软件中断测试---------------------------------------------
#endif

