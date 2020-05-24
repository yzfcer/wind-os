/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_core.h
**Author: Jason Zhou
**Last Date: 2012.09.26
**Description: wind os的核心启动和线程调度功能
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2012.09.26
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2012.10.20
** Description: 
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
#endif // #ifdef __cplusplus

#define IRQ_NEST_DEPTH 32
typedef struct __w_core_var_s
{
    volatile w_uint32_t idle_cnt;//空闲计算器
    volatile w_uint32_t idle_cnt_max;//在一段时间内的idle任务的计数值
    volatile w_int32_t irq_nest;//全局的中断嵌套计数值
    volatile w_int32_t switch_nest;//全局的禁止线程切换嵌套计数值
    volatile w_uint32_t sec_count;
    volatile w_uint32_t ms_cnt;//毫秒计时
    volatile w_uint32_t ticks_cnt;//tick计时
    volatile w_uint32_t cpu_usage;
    volatile w_int32_t irq_mask_idx;
    volatile w_irqreg_t irq_mask[IRQ_NEST_DEPTH];
}w_core_var_s;

extern w_core_var_s g_core;//内核相关的参数集
extern volatile w_bool_t gwind_start_flag;//开始调度的标志
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void);

w_err_t wind_get_core_var(w_core_var_s *core_var);
w_uint32_t wind_get_tick(void);
w_uint32_t wind_get_seconds(void);

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
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_CORE_H__

