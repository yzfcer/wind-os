/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_watchdog.c
**创   建   人: 周江村
**最后修改日期: 2018.08.12
**描        述: 线程粒度的看门狗，主要是监控线程得不到CPU而饿死
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018.08.12
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2018.08.12
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_watchdog.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"
#include "wind_timer.h"
#include "wind_os_hwif.h"



#if WIND_WATCHDOG_SUPPORT
//extern void _wind_thread_dispatch(void);
static WIND_POOL(watchdogpool,WIND_WATCHDOG_MAX_NUM,sizeof(watchdog_s));

static __INLINE__ watchdog_s *watchdog_malloc(void)
{
    return (watchdog_s*)wind_pool_malloc(watchdogpool);
}

static __INLINE__ w_err_t watchdog_free(void *watchdog)
{
    return wind_pool_free(watchdogpool,watchdog);
}

static void watchdog_timer(void * arg)
{
    dnode_s *dnode;
    watchdog_s *watchdog;
    wind_disable_switch();
    foreach_node(dnode,&g_core.watchdoglist)
    {
        watchdog = DLIST_OBJ(dnode,watchdog_s,watchdognode);
        wind_disable_interrupt();
        watchdog->time_cur --;
        wind_enable_interrupt();
        if(watchdog->time_cur < 0)
        {
            if(watchdog->flag == WDOG_WARN)
                wind_printf("watchdog:thread %s is timeout.\r\n",watchdog->thread->name);
            else if(watchdog->flag == WDOG_RESET)
               wind_system_reset();
            watchdog->time_cur = watchdog->time_max;
        }
    }
    wind_enable_switch();
}

w_err_t _wind_watchdog_mod_init(void)
{
    w_err_t err;
    timer_s *timer;
    err = wind_pool_create("watchdog",watchdogpool,sizeof(watchdogpool),sizeof(watchdog_s));
    WIND_ASSERT_RETURN(err == ERR_OK,err);
    timer = wind_timer_create("watchdog",1000,watchdog_timer,NULL,B_TRUE);
    WIND_ASSERT_RETURN(timer != NULL,ERR_FAIL);
    return ERR_OK;
}

watchdog_s *wind_watchdog_get(const char *name)
{
    watchdog_s *watchdog;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.watchdoglist)
    {
        watchdog = DLIST_OBJ(dnode,watchdog_s,watchdognode);
        if(wind_strcmp(name,watchdog->name) == 0)
        {
            wind_enable_switch();
            return watchdog;
        }
    }
    wind_enable_switch();
    return NULL;
}

watchdog_s *wind_watchdog_create(const char *name,w_uint32_t flag,w_int16_t timeout_1s)
{
    watchdog_s *watchdog;
    wind_notice("create watchdog:%s",name);
    watchdog = watchdog_malloc();
    WIND_ASSERT_RETURN(watchdog != NULL,NULL);
    WIND_ASSERT_RETURN(timeout_1s > 0,NULL);
    watchdog->magic = WIND_WATCHDOG_MAGIC;
    watchdog->flag = flag;
    watchdog->name = name;
    DNODE_INIT(watchdog->watchdognode);
    watchdog->time_cur = timeout_1s;
    watchdog->time_max = timeout_1s;
    watchdog->thread = wind_thread_current();
    wind_disable_switch();
    dlist_insert_tail(&g_core.watchdoglist,&watchdog->watchdognode);
    wind_enable_switch();
    return watchdog;
}


w_err_t wind_watchdog_destroy(watchdog_s *watchdog)
{
    w_err_t err;
    WIND_ASSERT_RETURN(watchdog != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,ERR_INVALID_PARAM);
    wind_notice("destroy watchdog:%s",watchdog->name);
    wind_disable_switch();
    dlist_remove(&g_core.watchdoglist,&watchdog->watchdognode);
    wind_enable_switch();
    watchdog->magic = 0;
    watchdog->thread = NULL;
    err = watchdog_free(watchdog);
    return err;    
}

w_err_t wind_watchdog_feed(watchdog_s *watchdog)
{
    WIND_ASSERT_RETURN(watchdog != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,ERR_INVALID_PARAM);
    wind_disable_switch();
    watchdog->time_cur = watchdog->time_max;
    wind_enable_switch();
    return ERR_OK;
}



w_err_t wind_watchdog_print(dlist_s *list)
{
    dnode_s *dnode;
    watchdog_s *watchdog;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nwatchdog list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-12s %-12s\r\n","watchdog","timeout_max","timeout_cur");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        watchdog = (watchdog_s *)DLIST_OBJ(dnode,watchdog_s,watchdognode);
        wind_printf("%-16s %-12d %-12d\r\n",
            watchdog->name,watchdog->time_max,watchdog->time_cur);
    }
    wind_print_space(5);
    return ERR_OK;
}

#endif

