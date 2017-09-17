/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_timer.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的以tick为精度的代定时器的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_config.h"
#include "wind_timer.h"
#include "wind_list.h"
#include "wind_debug.h"
#include "wind_err.h"
#include "wind_stat.h"
#include "wind_var.h"
#if WIND_TTIMER_SUPPORT > 0
static ptimer_s ttimer_malloc(void)
{
    ptimer_s timer;
    timer = wind_core_alloc(STAT_TTIMER);
    if(timer == NULL)
    {
        return NULL;
    }
    timer->used = B_TRUE;
    timer->running = B_FALSE;
    return timer;
}
w_err_t ttimer_free(ptimer_s timer)
{
    if(timer == NULL)
        return ERR_NULL_POINTER;
    wind_close_interrupt();
    
    timer->running = B_FALSE;
    timer->used = B_FALSE;
    timer->count = 0;
    timer->init_count = 0;
    timer->timercallback = NULL;
    wind_open_interrupt();
    return ERR_OK;
}


ptimer_s wind_timer_create(w_uint32_t t_ms,softtimer_fn func,void *arg,w_bool_t run)
{
    ptimer_s timer;
    pnode_s pnode;
    w_int32_t count = t_ms / 10;
    if(count <= 0)
        count = 1;
    WIND_INFO("creating tick timer:%d ms\r\n",t_ms);
    if(timercallback == NULL)
    {
        WIND_ERROR("wind_timer_create err 1\r\n");
        return NULL;
    }
    wind_close_interrupt();
    timer = ttimer_malloc();
    if(!timer)
    {
        wind_open_interrupt();
        WIND_ERROR("wind_timer_create err 2\r\n");
        return NULL;
    }
    pnode = wind_node_malloc(CORE_TYPE_TTIMER);
    if(!pnode)
    {
        wind_timer_free(timer);
        wind_open_interrupt();
        WIND_ERROR("wind_timer_create err 3\r\n");
        return NULL;
    }
    wind_open_interrupt();
    WIND_INFO("wind_timer_create OK\r\n");

    timer->count = count;
    timer->init_count = count;
    timer->running = run;
    timer->arg = arg;
    timer->handle = func;
    wind_node_bindobj(pnode,CORE_TYPE_TTIMER,t_ms,timer);
    wind_list_insert(&g_core.ttmerlist,pnode);
    return timer; 
}

w_err_t wind_timer_start(ptimer_s pttimer)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->running = B_TRUE;
    return ERR_OK;
}

w_err_t wind_timer_stop(ptimer_s pttimer)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->running = B_FALSE;
    return ERR_OK;
}

w_err_t wind_timer_free(ptimer_s pttimer)
{
    pnode_s pnode;
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    wind_close_interrupt();    
    pnode = wind_list_search(&g_core.ttmerlist,pttimer);
    if(pnode == NULL)
    {
        wind_open_interrupt();    
        return ERR_INVALID_PARAM;
    }    
    wind_list_remove(&g_core.ttmerlist,pnode);
    wind_node_free(pnode);
    ttimer_free(pttimer);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_timer_setticks(ptimer_s pttimer,w_uint32_t ticks)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->count = ticks;
    return ERR_OK;
}

extern void wind_timer_handler(void)
{
    ptimer_s ptmr;
    pnode_s tmpttimerlist = g_core.ttmerlist.head;
    while(tmpttimerlist)
    {
        ptmr = (ptimer_s)(tmpttimerlist->obj);
        if(ptmr->count > 0)
            ptmr->count --;
        if(ptmr->count == 0 && ptmr->running)
        {
            ptmr->timercallback();
            ptmr->count = ptmr->init_count;
        }
        tmpttimerlist = tmpttimerlist->next;
    }
}
#endif //#if WIND_TTIMER_SUPPORT > 0




