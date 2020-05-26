/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_watchdog.c
** Author      : Jason Zhou
** Last Date   : 2018.08.12
** Description : Thread granularity watchdog is mainly used to monitor the starvation of threads without CPU
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018.08.12
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2018.08.12
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_watchdog.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"
#include "wind_timer.h"
#include "wind_board_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_WATCHDOG_SUPPORT
#define NODE_TO_WATCHDOG(node) (w_watchdog_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_watchdog_s*)0)->obj.objnode)))
static w_dlist_s watchdoglist;
static WIND_POOL(watchdogpool,WIND_WATCHDOG_MAX_NUM,sizeof(w_watchdog_s));

static __INLINE__ w_watchdog_s *watchdog_malloc(void)
{
    return (w_watchdog_s*)wind_pool_malloc(watchdogpool);
}

static __INLINE__ w_err_t watchdog_free(void *watchdog)
{
    return wind_pool_free(watchdogpool,watchdog);
}

static void watchdog_timer(w_timer_s *timer,void * arg)
{
    w_dnode_s *dnode;
    w_watchdog_s *watchdog;
    wind_disable_switch();
    foreach_node(dnode,&watchdoglist)
    {
        watchdog = NODE_TO_WATCHDOG(dnode);
        if(!IS_F_WATCHDOG_ENABLE(watchdog))
            continue;
        wind_disable_interrupt();
        watchdog->time_cur --;
        wind_enable_interrupt();
        if(watchdog->time_cur < 0)
        {
            if(IS_F_WATCHDOG_WARN(watchdog))
                wind_printf("watchdog:thread %s is timeout.\r\n",watchdog->thread->name);
            if(IS_F_WATCHDOG_RESET(watchdog))
                wind_system_reset();
            watchdog->time_cur = watchdog->time_max;
        }
    }
    wind_enable_switch();
}

w_err_t _wind_watchdog_mod_init(void)
{
    w_err_t err;
    w_timer_s *timer;
    DLIST_INIT(watchdoglist);
    err = wind_pool_create("watchdog",watchdogpool,sizeof(watchdogpool),sizeof(w_watchdog_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    timer = wind_timer_create("watchdog",1000,watchdog_timer,W_NULL,F_TIMER_RUN | F_TIMER_REPEAT);
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}

w_watchdog_s *wind_watchdog_get(const char *name)
{
    return (w_watchdog_s*)wind_obj_get(name,&watchdoglist);
}

w_err_t wind_watchdog_init(w_watchdog_s *watchdog,const char *name,w_int16_t timeout_1s)
{
    wind_notice("init watchdog:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(watchdog > 0,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timeout_1s > 0,W_ERR_INVALID);
    watchdog->time_cur = timeout_1s;
    watchdog->time_max = timeout_1s;
    watchdog->thread = wind_thread_current();

    wind_obj_init(&watchdog->obj,WIND_WATCHDOG_MAGIC,name,&watchdoglist);
    wind_watchdog_setflag(watchdog,F_WATCHDOG_ENABLE|F_WATCHDOG_WARN);
    return W_ERR_OK;
}


w_watchdog_s *wind_watchdog_create(const char *name,w_int16_t timeout_1s)
{
    w_err_t err;
    w_watchdog_s *watchdog;
    watchdog = watchdog_malloc();
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_NULL);
    err = wind_watchdog_init(watchdog,name,timeout_1s);
    if(err == W_ERR_OK)
    {
        SET_F_WATCHDOG_POOL(watchdog);
        return watchdog;
    }
    watchdog_free(watchdog);
    return W_NULL;
}


w_err_t wind_watchdog_destroy(w_watchdog_s *watchdog)
{
    w_err_t err;
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->obj.magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    wind_notice("destroy watchdog:%s",wind_obj_name(&watchdog->obj));
    err = wind_obj_deinit(&watchdog->obj,WIND_WATCHDOG_MAGIC,&watchdoglist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    if(IS_F_WATCHDOG_POOL(watchdog))
        watchdog_free(watchdog);
    return W_ERR_OK;    
}

w_err_t wind_watchdog_feed(w_watchdog_s *watchdog)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->obj.magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    watchdog->time_cur = watchdog->time_max;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_watchdog_setflag(w_watchdog_s *watchdog,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->obj.magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    if(flag & F_WATCHDOG_ENABLE)
        SET_F_WATCHDOG_ENABLE(watchdog);
    if(flag & F_WATCHDOG_WARN)
        SET_F_WATCHDOG_WARN(watchdog);
    if(flag & F_WATCHDOG_RESET)
        SET_F_WATCHDOG_RESET(watchdog);
    return W_ERR_OK;
}

w_err_t wind_watchdog_clrflag(w_watchdog_s *watchdog,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->obj.magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    if(flag & F_WATCHDOG_ENABLE)
        CLR_F_WATCHDOG_ENABLE(watchdog);
    if(flag & F_WATCHDOG_WARN)
        CLR_F_WATCHDOG_WARN(watchdog);
    if(flag & F_WATCHDOG_RESET)
        CLR_F_WATCHDOG_RESET(watchdog);
    return W_ERR_OK;
}

w_err_t wind_watchdog_print_list(void)
{
    return wind_obj_print_list(&watchdoglist);
}

w_err_t wind_watchdog_print_detail(void)
{
    w_dnode_s *dnode;
    w_watchdog_s *watchdog;
    w_dlist_s *list = &watchdoglist;
    wind_printf("\r\n\r\nwatchdog list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-12s %-12s\r\n","watchdog","timeout_max","timeout_cur");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        watchdog = NODE_TO_WATCHDOG(dnode);
        wind_printf("%-16s %-12d %-12d\r\n",
            watchdog->obj.name,watchdog->time_max,watchdog->time_cur);
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif // #if WIND_WATCHDOG_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
