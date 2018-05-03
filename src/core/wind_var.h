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
#include "wind_type.h"
#include "wind_dlist.h"
#if 0
#include "wind_thread.h"
#include "wind_pipe.h"
#include "wind_msgbox.h"
#include "wind_sem.h"
#include "wind_mutex.h"
#include "wind_timer.h"
#include "wind_pool.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif
typedef struct __core_var_s
{
    //内核对象列表
    dlist_s threadlist;
    dlist_s sleeplist;
    dlist_s poollist;
#if WIND_SEM_SUPPORT
    dlist_s semlist;
#endif
#if WIND_MUTEX_SUPPORT
    dlist_s mutexlist;
#endif
#if WIND_MSGBOX_SUPPORT
    dlist_s msgboxlist;
#endif
#if WIND_TIMER_SUPPORT
    dlist_s timerlist;
#endif
#if WIND_HEAP_SUPPORT
    dlist_s heaplist;
#endif
#if WIND_PIPE_SUPPORT
    dlist_s pipelist;
#endif
#if WIND_CONSOLE_SUPPORT
    dlist_s cmdlist;
#endif
#if WIND_DRVFRAME_SUPPORT
    dlist_s devlist;
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    dlist_s blkdevlist;
#endif
#if WIND_FS_SUPPORT
    dlist_s fslist;
    dlist_s filelist;
#endif
    
    //volatile w_bool_t run_falg;//多线程调度开始的标志
    w_bool_t usrthren;/*用户线程允许创建的标志 */
    
    volatile w_uint32_t idle_cnt;//空闲计算器
    w_uint32_t ticks_cnt;//tick计时
    w_uint32_t idle_cnt_max;//在一段时间内的idle任务的计数值
    w_uint32_t cpu_usage;
}core_var_s;
extern core_var_s g_core;//内核相关的参数集
extern volatile w_bool_t gwind_start_flag;//开始调度的标志

extern w_pstack_t *gwind_high_stack;
extern w_pstack_t *gwind_cur_stack;

void _wind_corevar_init(void);

//CPU的总体占用率
#define WIND_CPU_USAGE (g_core.cpu_usage)

#define IDLE_CNT_PER_SEC (g_core.idle_cnt_max)

#define RUN_FLAG (gwind_start_flag)

#define TICKS_CNT (g_core.ticks_cnt)


#ifdef __cplusplus
}
#endif


#endif

