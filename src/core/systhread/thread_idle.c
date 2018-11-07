/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: thread_idle.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 空闲线程
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_thread.h"

#define IDLE_STK_SIZE 64
static w_stack_t idlestk[IDLE_STK_SIZE];//空闲任务堆栈
static w_err_t thread_idle(w_int32_t argc,char **argv)
{    
    while(1)
    {
        g_core.idle_cnt ++;
    }
}


w_err_t _create_thread_idle(void)	
{
    w_thread_s *thread;
    thread = wind_thread_create("idle",thread_idle,
                    0,W_NULL,PRIO_LOW,idlestk,IDLE_STK_SIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,32767);
    return W_ERR_OK;
}

