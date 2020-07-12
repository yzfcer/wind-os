/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_daemon.c
** Author      : Jason Zhou
** Last Date   : 2019.02.23
** Description : wind-os thread guard function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.02.23
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_daemon.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_DAEMON_SUPPORT
#define NODE_TO_DAEMON(node) NODEPTR_TO_ITEMPTR(node,w_daemon_s,obj.objnode)
static w_dlist_s daemonlist;
WIND_POOL(daemonpool,WIND_DAEMON_MAX_NUM,sizeof(w_daemon_s));

static w_daemon_s *daemon_malloc(void)
{
    return (w_daemon_s *)wind_pool_malloc(daemonpool);
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


w_err_t _wind_daemon_period_check(void)
{
    w_dnode_s *dnode;
    w_daemon_s *daemon;
    w_thread_s *thread;
    wind_disable_switch();
    foreach_node(dnode,&daemonlist)
    {
        daemon = NODE_TO_DAEMON(dnode);
        if(!IS_F_OBJ_ENABLE(daemon->obj))
            continue;
        thread = wind_thread_get(daemon->obj.name);
        if(thread == W_NULL)
        {
            wind_notice("***check thread %s is dead,now restart",daemon->obj.name);
            daemon->daemon_func();
        }
    }
    wind_enable_switch();
    return W_ERR_OK;
}

w_daemon_s *wind_daemon_get(const char *name)
{
    return (w_daemon_s*)wind_obj_get(name,&daemonlist);
}

w_err_t wind_daemon_init(w_daemon_s *daemon,const char *name,w_daemon_fn daemon_func)
{
    w_thread_s *thread;
    wind_notice("init daemon:%s",name?name:"null");
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(daemon_func != W_NULL,W_ERR_NULL_PTR);
    thread = wind_thread_get(name);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_INVALID);
    wind_thread_setflag(thread, F_THREAD_DAEMON | F_THREAD_SYSTEM);
    daemon->daemon_func = daemon_func;
    wind_obj_init(&daemon->obj,WIND_DAEMON_MAGIC,name,&daemonlist);
    SET_F_OBJ_ENABLE(daemon->obj);
    return W_ERR_OK;
}

w_daemon_s *wind_daemon_create(const char *name,w_daemon_fn daemon_func)
{
    w_err_t err;
    w_daemon_s *new_daemon = (w_daemon_s *)W_NULL;
    w_daemon_s *old_daemon = (w_daemon_s *)W_NULL;
    do
    {
        err = W_ERR_OK;
        old_daemon = wind_daemon_get(name);
        WIND_ASSERT_BREAK(old_daemon == W_NULL,W_ERR_REPEAT,"daemon obj has been exsit");
        new_daemon = daemon_malloc();
        WIND_ASSERT_BREAK(new_daemon != W_NULL,W_ERR_MEM,"alloc daemon obj failed");
        err = wind_daemon_init(new_daemon,name,daemon_func);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init daemon obj failed");
        SET_F_OBJ_POOL(new_daemon->obj);
    }while(0);
    if(err != W_ERR_OK)
    {
        if(new_daemon != W_NULL)
            daemon_free(new_daemon);
        new_daemon = (w_daemon_s *)W_NULL;
    }
    return new_daemon;
}



w_err_t wind_daemon_destroy(w_daemon_s *daemon)
{
    w_err_t err;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(daemon != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(daemon->obj.magic == WIND_DAEMON_MAGIC,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        wind_notice("destroy daemon:%s",wind_obj_name(&daemon->obj));
        err = wind_obj_deinit(&daemon->obj,WIND_DAEMON_MAGIC,&daemonlist);
        WIND_ASSERT_BREAK(err == W_ERR_OK, W_ERR_FAIL,"deinit damon obj failed");
        
        thread = wind_thread_get(daemon->obj.name);
        WIND_ASSERT_BREAK(thread != W_NULL,W_ERR_NO_OBJ,"thread is NOT exist");
        wind_thread_clrflag(thread,F_THREAD_DAEMON);
        
        if(IS_F_OBJ_POOL(daemon->obj))
            daemon_free(daemon);
    }while(0);
    return err;
}


w_err_t wind_daemon_print_list(void)
{
    return wind_obj_print_list(&daemonlist);
}


#endif  // #if WIND_DAEMON_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
