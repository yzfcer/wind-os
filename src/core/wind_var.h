/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_var.h / wind_var.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的内核相关的变量的集合
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.11.27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __w_core_var_s
{
    //内核对象列表
    

#if WIND_FS_SUPPORT
    w_dlist_s fslist;
    w_dlist_s filelist;
#endif
    
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

//CPU的总体占用率
#define WIND_CPU_USAGE (g_core.cpu_usage)

#define IDLE_CNT_PER_SEC (g_core.idle_cnt_max)

#define RUN_FLAG (gwind_start_flag)

#define TICKS_CNT (g_core.ticks_cnt)


#ifdef __cplusplus
}
#endif


#endif

