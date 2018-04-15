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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_dlist.h"
#include "wind_pipe.h"
#include "wind_msgbox.h"
#include "wind_sem.h"
#include "wind_mutex.h"
#include "wind_timer.h"
#include "wind_debug.h"
#include "wind_mpool.h"
#include "wind_var.h"
core_var_s g_core;
volatile w_bool_t gwind_start_flag = B_FALSE;

w_pstack_t *gwind_high_stack;
w_pstack_t *gwind_cur_stack;

void _wind_corevar_init(void)
{
    g_core.cpu_usage = 0;
    g_core.idle_cnt = 0;
    g_core.usrthren = B_FALSE;
    //g_core.run_falg = B_FALSE;
    g_core.ticks_cnt = 0;
    
    DLIST_INIT(g_core.threadlist);
    DLIST_INIT(g_core.sleeplist);
    DLIST_INIT(g_core.semlist);
    DLIST_INIT(g_core.mutexlist);
    DLIST_INIT(g_core.msgboxlist);
    DLIST_INIT(g_core.timerlist);
    DLIST_INIT(g_core.poollist);
    DLIST_INIT(g_core.heaplist);
    DLIST_INIT(g_core.pipelist);
    DLIST_INIT(g_core.devlist);
    DLIST_INIT(g_core.blkdevlist);
    
}

