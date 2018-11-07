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
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_pipe.h"
#include "wind_msgbox.h"
#include "wind_sem.h"
#include "wind_mutex.h"
#include "wind_timer.h"
#include "wind_pool.h"
#include "wind_debug.h"
//wind-os的基本全局参数和各种内核资源的链表头
w_core_var_s g_core;
//wind-os开始启动线程调度的标记
volatile w_bool_t gwind_start_flag = W_FALSE;
//高优先级线程栈指针
w_stack_t **gwind_high_stack;
//当前线程栈指针
w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.usrthren = W_FALSE;
    g_core.ticks_cnt = 0;
    
    DLIST_INIT(g_core.threadlist);
    DLIST_INIT(g_core.sleeplist);
    DLIST_INIT(g_core.poollist);





#if WIND_DRVFRAME_SUPPORT
    DLIST_INIT(g_core.devlist);
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    DLIST_INIT(g_core.blkdevlist);
#endif
#if WIND_FS_SUPPORT
    DLIST_INIT(g_core.fslist);
    DLIST_INIT(g_core.filelist);
#endif
    
}

