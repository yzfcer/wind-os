/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_timer.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的以tick为精度的代定时器的代码，非精确定时，绝对时间误差为一个tick周期
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
#include "wind_timer.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_TIMER_SUPPORT
#define NODE_TO_TIMER(node) (w_timer_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_timer_s*)0)->obj.objnode)))
static w_dlist_s timerlist;
static WIND_POOL(timerpool,WIND_TIMER_MAX_NUM,sizeof(w_timer_s));

static __INLINE__ w_timer_s *timer_malloc(void)
{
    return (w_timer_s*)wind_pool_malloc(timerpool);
}

static __INLINE__ w_err_t timer_free(void *timer)
{
    return wind_pool_free(timerpool,timer);
}


w_err_t _wind_timer_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(timerlist);
    err = wind_pool_create("timer",timerpool,sizeof(timerpool),sizeof(w_timer_s));
    return err;
}
w_timer_s* wind_timer_get(char *name)
{
    return (w_timer_s*)wind_obj_get(name,&timerlist);
}

w_err_t wind_timer_init(w_timer_s* timer,
                            const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint16_t flag)
{
    w_int32_t count;
    wind_notice("init timer:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(func != W_NULL,W_ERR_PTR_NULL);
    count = period_ms / TIMER_PERIOD;
    if(count <= 0)
        count = 1;
    wind_notice("init timer %s:%d ms",name,period_ms);

    timer->value = count;
    timer->period = count;
    
    timer->arg = arg;
    timer->handle = func;
    wind_obj_init(&timer->obj,WIND_TIMER_MAGIC,name,&timerlist);
    wind_timer_setflag(timer,flag);

    return W_ERR_OK;
}


w_timer_s* wind_timer_create(const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint16_t flag)
{
    w_err_t err;
    w_timer_s* timer;
    timer = timer_malloc();
    WIND_ASSERT_RETURN(timer != W_NULL,W_NULL);
    err = wind_timer_init(timer,name,period_ms,func,arg,flag);
    if(err == W_ERR_OK)
    {
        SET_F_TIMER_POOL(timer);
        return timer;
    }
    timer_free(timer);
    return W_NULL; 
}

w_err_t wind_timer_start(w_timer_s* timer)
{
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);    
    SET_F_TIMER_RUN(timer);
    return W_ERR_OK;
}

w_err_t wind_timer_stop(w_timer_s* timer)
{
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);    
    CLR_F_TIMER_RUN(timer);
    return W_ERR_OK;
}

w_err_t wind_timer_destroy(w_timer_s* timer)
{
    w_err_t err;
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);    
    wind_notice("destroy timer:%s",timer->obj.name != W_NULL?timer->obj.name:"null");
    err = wind_obj_deinit(&timer->obj,WIND_TIMER_MAGIC,&timerlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    if(IS_F_TIMER_POOL(timer))
        timer_free(timer);
    return W_ERR_OK;
}

w_err_t wind_timer_set_period(w_timer_s* timer,w_uint32_t period_ms)
{
    w_int32_t count;
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);
    count = period_ms / TIMER_PERIOD;
    if(count <= 0)
        count = 1;
    timer->period = count;
    timer->value = count;
    return W_ERR_OK;
}

w_err_t wind_timer_setflag(w_timer_s* timer,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);
    if(flag & F_TIMER_REPEAT)
        SET_F_TIMER_REPEAT(timer);
    if(flag & F_TIMER_RUN)
    {
        if(!IS_F_TIMER_RUN(timer))
        {
            timer->value = timer->period;
            SET_F_TIMER_RUN(timer);
        }
    }
    return W_ERR_OK;
}

w_err_t wind_timer_clrflag(w_timer_s* timer,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timer->obj.magic == WIND_TIMER_MAGIC,W_ERR_INVALID);
    if(flag & F_TIMER_REPEAT)
        CLR_F_TIMER_REPEAT(timer);
    if(flag & F_TIMER_RUN)
        SET_F_TIMER_RUN(timer);
    return W_ERR_OK;
}


w_err_t wind_timer_print(void)
{
    w_dnode_s *dnode;
    w_timer_s *timer;
    w_dlist_s *list = &timerlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\ntimer list:\r\n");
    wind_print_space(7);
    wind_printf("%-16s %-10s %-10s %-10s %-10s\r\n","timer","period","value","status","repeat");
    wind_print_space(7);

    foreach_node(dnode,list)
    {
        timer = NODE_TO_TIMER(dnode);
        wind_printf("%-16s %-10d %-10d %-10s %-10s\r\n",
            timer->obj.name,timer->period,timer->value,IS_F_TIMER_RUN(timer)?"running":"stop",
            IS_F_TIMER_REPEAT(timer)?"yes":"no");
    }
    wind_print_space(7);
    return W_ERR_OK;
}


void _wind_timer_event(void)
{
    w_timer_s* timer;
    w_dnode_s *dnode;
    w_dlist_s *list = &timerlist;
    foreach_node(dnode,list)
    {
        timer = NODE_TO_TIMER(dnode);
        if(timer->value > 0)
            timer->value --;
        if(timer->value == 0 && IS_F_TIMER_RUN(timer))
        {
            timer->handle(timer,timer->arg);
            if(IS_F_TIMER_REPEAT(timer))
                timer->value = timer->period;
            else
            {
                wind_timer_stop(timer);
            }
        }
    }
}

#endif //#if WIND_TIMER_SUPPORT


