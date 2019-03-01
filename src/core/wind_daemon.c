/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_daemon.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.02.23
**描        述: wind os的线程守护对象
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.02.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_daemon.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_DAEMON_SUPPORT
static w_dlist_s daemonlist;
WIND_POOL(daemonpool,WIND_DAEMON_MAX_NUM,sizeof(w_daemon_s));

static w_daemon_s *daemon_malloc(void)
{
    return wind_pool_malloc(daemonpool);
}

static w_err_t daemon_free(w_daemon_s *daemon)
{
    return wind_pool_free(daemonpool,daemon);
}


w_err_t _wind_daemon_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(daemonlist);
    err = wind_pool_create("daemon",daemonpool,sizeof(daemonpool),sizeof(w_daemon_s));
    return err;
}
w_err_t wind_daemon_setflag(w_daemon_s *daemon,w_int16_t flag)
{
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(daemon->magic == WIND_DAEMON_MAGIC,W_ERR_INVALID);
    if(flag & F_DAEMON_ENABLE)
        SET_F_DAEMON_ENABLE(daemon);
    return W_ERR_OK;
}
w_err_t wind_daemon_clrflag(w_daemon_s *daemon,w_int16_t flag)
{
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(daemon->magic == WIND_DAEMON_MAGIC,W_ERR_INVALID);
    if(flag & F_DAEMON_ENABLE)
        CLR_F_DAEMON_ENABLE(daemon);
    return W_ERR_OK;
}

w_err_t _wind_daemon_period_check(void)
{
    w_dnode_s *dnode;
    w_daemon_s *daemon;
    w_thread_s *thread;
    wind_disable_switch();
    foreach_node(dnode,&daemonlist)
    {
        daemon = DLIST_OBJ(dnode,w_daemon_s,daemonnode);
        if(!IS_F_DAEMON_ENABLE(daemon))
            continue;
        thread = wind_thread_get(daemon->name);
        if(thread == W_NULL)
        {
            wind_notice("***check thread %s is dead,now restart",daemon->name);
            daemon->daemon_func();
        }
    }
    wind_enable_switch();
    return W_ERR_OK;
}

w_daemon_s *wind_daemon_get(const char *name)
{
    w_daemon_s *daemon;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&daemonlist)
    {
        daemon = DLIST_OBJ(dnode,w_daemon_s,daemonnode);
        if(daemon->name && (wind_strcmp(name,daemon->name) == 0))
        {
            wind_enable_switch();
            return daemon;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_daemon_init(w_daemon_s *daemon,const char *name,w_daemon_fn daemon_func)
{
    w_thread_s *thread;
    wind_notice("init daemon:%s",name?name:"null");
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(daemon_func != W_NULL,W_ERR_PTR_NULL);
    thread = wind_thread_get(name);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_INVALID);
    wind_thread_setflag(thread,F_THREAD_DAEMON);
    daemon->magic = WIND_DAEMON_MAGIC;
    daemon->name = name;
    DNODE_INIT(daemon->daemonnode);
    daemon->daemon_func = daemon_func;
    daemon->flag = 0;
    CLR_F_DAEMON_POOL(daemon);
    wind_disable_interrupt();
    dlist_insert_tail(&daemonlist,&daemon->daemonnode);
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_daemon_s *wind_daemon_create(const char *name,w_daemon_fn daemon_func)
{
    w_err_t err;
    w_daemon_s *daemon;
    daemon = wind_daemon_get(name);
    if(daemon != W_NULL)
        return daemon;
    daemon = daemon_malloc();
    WIND_ASSERT_RETURN(daemon != W_NULL,W_NULL);
    err = wind_daemon_init(daemon,name,daemon_func);
    if(err == W_ERR_OK)
    {
        SET_F_DAEMON_POOL(daemon);
        return daemon;
    }
    daemon_free(daemon);
    return daemon;
}



w_err_t wind_daemon_destroy(w_daemon_s *daemon)
{
    w_thread_s *thread;
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(daemon->magic == WIND_DAEMON_MAGIC,W_ERR_INVALID);
    wind_notice("destroy daemon:%s",daemon->name?daemon->name:"null");
    wind_disable_interrupt();
    dlist_remove(&daemonlist,&daemon->daemonnode);
    wind_enable_interrupt();
    thread = wind_thread_get(daemon->name);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_INVALID);
    wind_thread_clrflag(thread,F_THREAD_DAEMON);
    daemon->magic = (~WIND_DAEMON_MAGIC);
    if(IS_F_DAEMON_POOL(daemon))
        daemon_free(daemon);
    return W_ERR_OK;
}


w_err_t wind_daemon_print(void)
{
    w_dnode_s *dnode;
    w_daemon_s *daemon;
    int cnt = 0;
    w_dlist_s *list = &daemonlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\ndaemon list:\r\n");
    
    foreach_node(dnode,list)
    {
        daemon = (w_daemon_s *)DLIST_OBJ(dnode,w_daemon_s,daemonnode);
        wind_printf("%-12s ",daemon->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif  //WIND_DAEMON_H__

