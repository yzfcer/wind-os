/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_core.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的核心启动和线程调度功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CORE_H__
#define WIND_CORE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct __w_core_var_s
{
    w_bool_t usrthren;/*用户线程允许创建的标志 */
    volatile w_uint32_t idle_cnt;//空闲计算器
    w_uint32_t ticks_cnt;//tick计时
    w_uint32_t idle_cnt_max;//在一段时间内的idle任务的计数值
    w_uint32_t cpu_usage;
}w_core_var_s;
extern w_core_var_s g_core;//内核相关的参数集
extern volatile w_bool_t gwind_start_flag;//开始调度的标志
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void);

#define WIND_CPU_USAGE (g_core.cpu_usage)
#define IDLE_CNT_PER_SEC (g_core.idle_cnt_max)
#define RUN_FLAG (gwind_start_flag)
#define TICKS_CNT (g_core.ticks_cnt)



void _wind_thread_dispatch(void);
void _wind_switchto_thread(w_thread_s *thread);

void wind_disable_switch(void);
void wind_enable_switch(void);

void wind_disable_interrupt(void);
void wind_enable_interrupt(void);

void wind_enter_irq(void);
void wind_exit_irq(void);


#ifdef __cplusplus
}
#endif

#endif

