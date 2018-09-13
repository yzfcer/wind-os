/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_watchdog.c
**��   ��   ��: �ܽ���
**����޸�����: 2018.08.12
**��        ��: �߳����ȵĿ��Ź�����Ҫ�Ǽ���̵߳ò���CPU������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2018.08.12
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2018.08.12
** �衡��: 
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
static WIND_POOL(watchdogpool,WIND_WATCHDOG_MAX_NUM,sizeof(w_watchdog_s));

static __INLINE__ w_watchdog_s *watchdog_malloc(void)
{
    return (w_watchdog_s*)wind_pool_malloc(watchdogpool);
}

static __INLINE__ w_err_t watchdog_free(void *watchdog)
{
    return wind_pool_free(watchdogpool,watchdog);
}

static void watchdog_timer(void * arg)
{
    w_dnode_s *dnode;
    w_watchdog_s *watchdog;
    wind_disable_switch();
    foreach_node(dnode,&g_core.watchdoglist)
    {
        watchdog = DLIST_OBJ(dnode,w_watchdog_s,watchdognode);
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
    w_timer_s *timer;
    err = wind_pool_create("watchdog",watchdogpool,sizeof(watchdogpool),sizeof(w_watchdog_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    timer = wind_timer_create("watchdog",1000,watchdog_timer,W_NULL,W_TRUE);
    WIND_ASSERT_RETURN(timer != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}

w_watchdog_s *wind_watchdog_get(const char *name)
{
    w_watchdog_s *watchdog;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.watchdoglist)
    {
        watchdog = DLIST_OBJ(dnode,w_watchdog_s,watchdognode);
        if(wind_strcmp(name,watchdog->name) == 0)
        {
            wind_enable_switch();
            return watchdog;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_watchdog_s *wind_watchdog_create(const char *name,w_uint32_t flag,w_int16_t timeout_1s)
{
    w_watchdog_s *watchdog;
    wind_notice("create watchdog:%s",name);
    watchdog = watchdog_malloc();
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_NULL);
    WIND_ASSERT_RETURN(timeout_1s > 0,W_NULL);
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


w_err_t wind_watchdog_destroy(w_watchdog_s *watchdog)
{
    w_err_t err;
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(watchdog->magic == WIND_WATCHDOG_MAGIC,W_ERR_INVALID);
    wind_notice("destroy watchdog:%s",watchdog->name);
    wind_disable_switch();
    dlist_remove(&g_core.watchdoglist,&watchdog->watchdognode);
    wind_enable_switch();
    watchdog->magic = 0;
    watchdog->thread = W_NULL;
    err = watchdog_free(watchdog);
    return err;    
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



w_err_t wind_watchdog_print(w_dlist_s *list)
{
    w_dnode_s *dnode;
    w_watchdog_s *watchdog;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nwatchdog list as following:\r\n");
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

