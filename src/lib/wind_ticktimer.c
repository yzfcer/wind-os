/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_ticktimer.h
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
#include "wind_types.h"
#include "wind_config.h"
#include "wind_ticktimer.h"
#include "wind_list.h"
#include "wind_debug.h"
#include "wind_err.h"
#include "wind_stat.h"
#include "wind_var.h"
#if WIND_TTIMER_SUPPORT > 0
static pttimer_s ttimer_malloc(void)
{
    pttimer_s pttimer;
    pttimer = wind_core_alloc(STAT_TTIMER);
    if(pttimer == NULL)
    {
        return NULL;
    }
    pttimer->used = B_TRUE;
    pttimer->running = B_FALSE;
    return pttimer;
}
w_err_t ttimer_free(pttimer_s ttimer)
{
    if(ttimer == NULL)
        return ERR_NULL_POINTER;
    wind_close_interrupt();
    
    ttimer->running = B_FALSE;
    ttimer->used = B_FALSE;
    ttimer->count = 0;
    ttimer->inittick = 0;
    ttimer->timercallback = NULL;
    wind_open_interrupt();
    return ERR_OK;
}


pttimer_s wind_ttimer_create(w_uint32_t ticks,void (*timercallback)(void),w_bool_t run)
{
    pttimer_s ttimer;
    pnode_s pnode;
    WIND_INFO("creating tick timer:%d ms\r\n",ticks);
    if(timercallback == NULL)
    {
        WIND_ERROR("wind_ttimer_create err 1\r\n");
        return NULL;
    }
    wind_close_interrupt();
    ttimer = ttimer_malloc();
    if(!ttimer)
    {
        wind_open_interrupt();
        WIND_ERROR("wind_ttimer_create err 2\r\n");
        return NULL;
    }
    pnode = wind_node_malloc(CORE_TYPE_TTIMER);
    if(!pnode)
    {
        wind_ttimer_free(ttimer);
        wind_open_interrupt();
        WIND_ERROR("wind_ttimer_create err 3\r\n");
        return NULL;
    }
    wind_open_interrupt();
    WIND_INFO("wind_ttimer_create OK\r\n");

    ttimer->count = ticks;
    ttimer->inittick = ticks;
    ttimer->running = run;
    ttimer->timercallback = timercallback;
    wind_node_bindobj(pnode,CORE_TYPE_TTIMER,ticks,ttimer);
    wind_list_insert(&g_core.ttmerlist,pnode);
    return ttimer; 
}

w_err_t wind_ttimer_start(pttimer_s pttimer)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    //pttmr = ttimer_search(htimer);
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->running = B_TRUE;
    return ERR_OK;
}

w_err_t wind_ttimer_stop(pttimer_s pttimer)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->running = B_FALSE;
    return ERR_OK;
}

w_err_t wind_ttimer_free(pttimer_s pttimer)
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

w_err_t wind_ttimer_setticks(pttimer_s pttimer,w_uint32_t ticks)
{
    if(pttimer == NULL)
        return ERR_NULL_POINTER;
    if(!pttimer->used)
        return ERR_COMMAN;
    pttimer->count = ticks;
    return ERR_OK;
}

extern void ticktimerhandler(void)
{
    pttimer_s pttmr;
    pnode_s tmpttimerlist = g_core.ttmerlist.head;
    while(tmpttimerlist)
    {
        pttmr = (pttimer_s)(tmpttimerlist->obj);
        if(pttmr->count > 0)
            pttmr->count --;
        if(pttmr->count == 0 && pttmr->running)
        {
            pttmr->timercallback();
            pttmr->count = pttmr->inittick;
        }
        tmpttimerlist = tmpttimerlist->next;
    }
}
#endif //#if WIND_TTIMER_SUPPORT > 0




