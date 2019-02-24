/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_watchdog.c
**��   ��   ��: Jason Zhou
**����޸�����: 2018.08.12
**��        ��: �߳����ȵĿ��Ź�����Ҫ�Ǽ���̵߳ò���CPU������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018.08.12
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2018.08.12
** �衡��: 
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
#include "wind_os_hwif.h"



#if WIND_WATCHDOG_SUPPORT
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
        watchdog = DLIST_OBJ(dnode,w_watchdog_s,watchdognode);
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
    w_watchdog_s *watchdog;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&watchdoglist)
    {
        watchdog = DLIST_OBJ(dnode,w_watchdog_s,watchdognode);
        if(watchdog->name && (wind_strcmp(name,watchdog->name) == 0))
        {
            wind_enable_switch();
            return watchdog;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_watchdog_init(w_watchdog_s *watchdog,const char *name,w_int16_t timeout_1s)
{
    wind_notice("init watchdog:%s",name);
    WIND_ASSERT_RETURN(watchdog > 0,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(timeout_1s > 0,W_ERR_INVALID);
    watchdog->magic = WIND_WATCHDOG_MAGIC;
    watchdog->flag = 0;
    CLR_F_WATCHDOG_POOL(watchdog);
    SET_F_WATCHDOG_ENABLE(watchdog);
    SET_F_WATCHDOG_WARN(watchdog);
    watchdog->name = name;
    DNODE_INIT(watchdog->watchdognode);
    watchdog->time_cur = timeout_1s;
    watchdog->time_max = timeout_1s;
    watchdog->thread = wind_thread_current();
    wind_disable_switch();
    dlist_insert_tail(&watchdoglist,&watchdog->watchdognode);
    wind_enable_switch();
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
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    wind_notice("destroy watchdog:%s",watchdog->name);
    wind_disable_switch();
    dlist_remove(&watchdoglist,&watchdog->watchdognode);
    wind_enable_switch();
    watchdog->magic = (~WIND_WATCHDOG_MAGIC);
    watchdog->thread = W_NULL;
    if(IS_F_WATCHDOG_POOL(watchdog))
        watchdog_free(watchdog);
    return W_ERR_OK;    
}

w_err_t wind_watchdog_feed(w_watchdog_s *watchdog)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    watchdog->time_cur = watchdog->time_max;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_watchdog_setflag(w_watchdog_s *watchdog,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    if(flag & F_WATCHDOG_ENABLE)
        SET_F_WATCHDOG_ENABLE(watchdog);
    if(flag & F_WATCHDOG_WARN)
        CLR_F_WATCHDOG_WARN(watchdog);
    if(flag & F_WATCHDOG_RESET)
        CLR_F_WATCHDOG_RESET(watchdog);
    return W_ERR_OK;
}

w_err_t wind_watchdog_clrflag(w_watchdog_s *watchdog,w_uint16_t flag)
{
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    if(flag & F_WATCHDOG_ENABLE)
        CLR_F_WATCHDOG_ENABLE(watchdog);
    if(flag & F_WATCHDOG_WARN)
        CLR_F_WATCHDOG_WARN(watchdog);
    if(flag & F_WATCHDOG_RESET)
        CLR_F_WATCHDOG_RESET(watchdog);
    return W_ERR_OK;
}


w_err_t wind_watchdog_print(void)
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
        watchdog = (w_watchdog_s *)DLIST_OBJ(dnode,w_watchdog_s,watchdognode);
        wind_printf("%-16s %-12d %-12d\r\n",
            watchdog->name,watchdog->time_max,watchdog->time_cur);
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif

