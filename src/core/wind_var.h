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
#ifndef WIND_VAR_H__
#define WIND_VAR_H__

#include "wind_config.h"
#include "wind_types.h"
#include "wind_thread.h"
#include "wind_list.h"
#include "wind_pipe.h"
#include "wind_message.h"
#include "wind_sem.h"
#include "wind_lock.h"
#include "wind_ticktimer.h"
#include "wind_stat.h"
#include "wind_mpool.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct __core_var_s
{
    WIND_MPOOL(pcb,WIND_THREAD_MAX_NUM,sizeof(pcb_s));
    WIND_MPOOL(node,WIND_NODE_MAX_NUM,sizeof(node_s));
#if WIND_PIPE_SUPPORT > 0
    WIND_MPOOL(pipe,WIND_PIPE_MAX_NUM,sizeof(pipe_s));
#endif
#if WIND_MESSAGE_SUPPORT > 0
    WIND_MPOOL(msg,WIND_MESSAGE_MAX_NUM,sizeof(msg_s));
    WIND_MPOOL(mbox,WIND_MBOX_MAX_NUM,sizeof(mbox_s));
#endif
#if WIND_SEM_SUPPORT > 0
    WIND_MPOOL(sem,WIND_SEM_MAX_NUM,sizeof(sem_s));
#endif
#if WIND_TTIMER_SUPPORT > 0
    WIND_MPOOL(ttimer,WIND_TTIMER_MAX_NUM,sizeof(ttimer_s));
#endif
#if WIND_LOCK_SUPPORT > 0
    WIND_MPOOL(lock,WIND_LOCK_NUM,sizeof(lock_s));
#endif
    //定义一些堆栈以便创建程序在时使用
    WIND_MPOOL(stk128,WIND_STK128_MAX_NUM,128 * sizeof(stack_t));
    WIND_MPOOL(stk256,WIND_STK256_MAX_NUM,256 * sizeof(stack_t));
    WIND_MPOOL(stk512,WIND_STK512_MAX_NUM,512 * sizeof(stack_t));
    WIND_MPOOL(stk1024,WIND_STK1024_MAX_NUM,1024 * sizeof(stack_t));
    WIND_MPOOL(stk2048,WIND_STK2048_MAX_NUM,2048 * sizeof(stack_t));
    
    pnode_s pcblist;
    pnode_s ttmerlist;
    s16_t pcbcnt;//线程计数器
    u32_t idle_cnt;//空闲计算器
    bool_t usrprocen;/*用户线程允许创建的标志 */
    bool_t is_incore;
    
    u32_t g_wind_cpu_usage;
    pthread_s pmain,pidle,pstat,pctrl,pdaemon;
    
    u32_t idle_cnt_per_sec;//在一段时间内的idle任务的计数值
    volatile bool_t run_falg;//多线程调度开始的标志
    u32_t ticks_cnt;//tick计时

}core_var_s;
extern core_var_s g_core;//内核相关的参数集
extern volatile bool_t gwind_start_flag;//开始调度的标志
extern pthread_s gwind_cur_pcb;//当前的
extern pthread_s gwind_high_pcb;//最高优先级PCB指针

void wind_corevar_init(void);

void *wind_core_alloc(stat_e type);

err_t wind_core_free(stat_e type,void *block);

pstack_t wind_stack_alloc(u32_t size);

err_t wind_stack_free(pstack_t pstack);

//CPU的总体占用率
#define WIND_CPU_USAGE (g_core.g_wind_cpu_usage)

#define IDLE_CNT_PER_SEC (g_core.idle_cnt_per_sec)

#define RUN_FLAG (gwind_start_flag)

#define TICKS_CNT (g_core.ticks_cnt)

#define HIGH_PROC (gwind_high_pcb)
#ifdef __cplusplus
}
#endif


#endif

