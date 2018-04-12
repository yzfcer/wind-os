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
#include "wind_timer.h"
#include "wind_debug.h"
#include "wind_stati.h"
#include "wind_var.h"
#if WIND_TIMER_SUPPORT
static WIND_MPOOL(timerpool,WIND_TIMER_MAX_NUM,sizeof(timer_s));

static __INLINE__ timer_s *timer_malloc(void)
{
    return (timer_s*)wind_pool_alloc(timerpool);
}

static __INLINE__ w_err_t timer_free(void *timer)
{
    return wind_pool_free(timerpool,timer);
}


w_err_t wind_timer_init(void)
{
    w_err_t err;
    err = wind_pool_create("timer",timerpool,sizeof(timerpool),sizeof(timer_s));
    return err;
}


timer_s* wind_timer_create(w_uint32_t t_ms,softimer_fn func,void *arg,w_bool_t run)
{
    timer_s* timer;
    w_int32_t count = t_ms / TIMER_PERIOD;
    if(count <= 0)
        count = 1;
    wind_notice("creating soft timer:%d ms",t_ms);
    WIND_ASSERT_RETURN(func != NULL,NULL);
    timer = timer_malloc();
    WIND_ASSERT_RETURN(timer != NULL,NULL);
    wind_notice("creat soft timer OK");
    DNODE_INIT(timer->tmrnode);

    timer->count = count;
    timer->init_count = count;
    timer->running = run;
    timer->arg = arg;
    timer->handle = func;
    dlist_insert_tail(&g_core.ttmerlist,&timer->tmrnode);
    return timer; 
}

w_err_t wind_timer_start(timer_s* timer)
{
    WIND_ASSERT_RETURN(timer != NULL,ERR_NULL_POINTER);
    timer->running = B_TRUE;
    return ERR_OK;
}

w_err_t wind_timer_stop(timer_s* timer)
{
    WIND_ASSERT_RETURN(timer != NULL,ERR_NULL_POINTER);
    timer->running = B_FALSE;
    return ERR_OK;
}

w_err_t wind_timer_free(timer_s* timer)
{
    WIND_ASSERT_RETURN(timer != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    dlist_remove(&g_core.ttmerlist,&timer->tmrnode);
    timer_free(timer);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_timer_set_period(timer_s* timer,w_uint32_t t_ms)
{
    w_int32_t count = t_ms / TIMER_PERIOD;
    if(count <= 0)
        count = 1;
    WIND_ASSERT_RETURN(timer != NULL,ERR_NULL_POINTER);
    timer->init_count = count;
    timer->count = count;
    return ERR_OK;
}

void wind_timer_event(void)
{
    timer_s* ptmr;
    dnode_s *pdnode;
    foreach_node(pdnode,&g_core.ttmerlist)
    {
        ptmr = DLIST_OBJ(pdnode,timer_s,tmrnode);
        if(ptmr->count > 0)
            ptmr->count --;
        if(ptmr->count == 0 && ptmr->running)
        {
            ptmr->handle(ptmr->arg);
            ptmr->count = ptmr->init_count;
        }
    }
}

#endif //#if WIND_TIMER_SUPPORT


