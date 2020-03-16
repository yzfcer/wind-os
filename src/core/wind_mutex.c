/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mutex.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 系统的互斥锁
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.2
** 日　期: 2013.06.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.06.23
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_MUTEX_SUPPORT
#define NODE_TO_MUTEX(node) (w_mutex_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_mutex_s*)0)->obj.objnode)))
static w_dlist_s mutexlist;
static WIND_POOL(mutexpool,WIND_MUTEX_MAX_NUM,sizeof(w_mutex_s));

static __INLINE__ w_mutex_s *mutex_malloc(void)
{
    return (w_mutex_s*)wind_pool_malloc(mutexpool);
}

static __INLINE__ w_err_t mutex_free(void *mutex)
{
    return wind_pool_free(mutexpool,mutex);
}
w_err_t _wind_mutex_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(mutexlist);
    err = wind_pool_create("mutex",mutexpool,sizeof(mutexpool),sizeof(w_mutex_s));
    return err;
}

w_mutex_s *wind_mutex_get(const char *name)
{
    return (w_mutex_s*)wind_obj_get(name,&mutexlist);
}

w_err_t wind_mutex_init(w_mutex_s *mutex,const char *name)
{
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    mutex->nest = 0;
    mutex->owner = W_NULL;
    DLIST_INIT(mutex->waitlist);
    wind_obj_init(&mutex->obj,WIND_MUTEX_MAGIC,name,&mutexlist);
    return W_ERR_OK;
}

w_mutex_s *wind_mutex_create(const char *name)
{
    w_err_t err;
    w_mutex_s *mutex;
    mutex = mutex_malloc();
    WIND_ASSERT_RETURN(mutex != W_NULL,W_NULL);
    err = wind_mutex_init(mutex,name);
    if(err == W_ERR_OK)
    {
        SET_F_MUTEX_POOL(mutex);
        return mutex;
    }
    mutex_free(mutex);
    return W_NULL;
}

w_err_t wind_mutex_trydestroy(w_mutex_s *mutex)
{
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->obj.magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    WIND_ASSERT_TODO_RETURN(!IS_F_MUTEX_LOCKED(mutex),wind_enable_interrupt(),W_ERR_FAIL);
    wind_mutex_destroy(mutex);
    wind_enable_switch();
    return W_ERR_OK;    
}

w_err_t wind_mutex_destroy(w_mutex_s *mutex)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    err = wind_obj_deinit(&mutex->obj,WIND_MUTEX_MAGIC,&mutexlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    wind_disable_switch();
    foreach_node(dnode,&mutex->waitlist)
    {
        dlist_remove(&mutex->waitlist,dnode);
        thread = PRIDNODE_TO_THREAD(dnode,suspendnode);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_LOCK;
    }
    wind_enable_switch();
    if(IS_F_MUTEX_POOL(mutex))
        mutex_free(mutex);
    return W_ERR_OK;
}


w_err_t wind_mutex_lock(w_mutex_s *mutex)
{
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->obj.magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    thread = wind_thread_current();

    if (!IS_F_MUTEX_LOCKED(mutex))
    {
        SET_F_MUTEX_LOCKED(mutex);
        mutex->nest ++;
        mutex->owner = wind_thread_current();
        wind_enable_switch();
        return W_ERR_OK; 
    }
    if(thread == mutex->owner)
    {
        if(mutex->nest < 65535)
            mutex->nest ++;
        wind_enable_switch();
        return W_ERR_OK; 
    }
    
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_LOCK;
    thread->sleep_ticks = 0x7fffffff;
    
    dlist_insert_prio(&mutex->waitlist,&thread->suspendnode,thread->prio);
    wind_enable_switch();
    _wind_thread_dispatch();
    return W_ERR_OK;
}

w_err_t wind_mutex_trylock(w_mutex_s *mutex)
{
    w_err_t err;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->obj.magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    if (!IS_F_MUTEX_LOCKED(mutex))
    {
        SET_F_MUTEX_LOCKED(mutex);
        mutex->owner = wind_thread_current();
        err = W_ERR_OK; 
    }
    else
        err = W_ERR_FAIL; 
    wind_enable_switch();
    return err;
}


w_err_t wind_mutex_unlock(w_mutex_s *mutex)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->obj.magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    WIND_ASSERT_TODO_RETURN(IS_F_MUTEX_LOCKED(mutex),wind_enable_switch(),W_ERR_OK);
    thread = wind_thread_current();
    WIND_ASSERT_TODO_RETURN(mutex->owner == thread,wind_enable_switch(),W_ERR_FAIL);
    if(mutex->nest > 0)
        mutex->nest --;
    if(mutex->nest > 0)
    {
        wind_enable_switch();
        return W_ERR_OK;
    }
    dnode = dlist_head(&mutex->waitlist);
    if (dnode == W_NULL)
    {
        CLR_F_MUTEX_LOCKED(mutex);
        mutex->owner = W_NULL;
        wind_enable_switch();
        return W_ERR_OK; 
    }

    dlist_remove_head(&mutex->waitlist);
    thread = PRIDNODE_TO_THREAD(dnode,suspendnode);
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_LOCK;
    mutex->owner = thread;
    wind_enable_switch();
    return W_ERR_OK;    
}


w_err_t wind_mutex_print(void)
{
    w_dnode_s *dnode;
    w_mutex_s *mutex;
    w_dlist_s *list = &mutexlist;
    wind_printf("\r\n\r\nmutex list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s \r\n","mutex","status","owner");
    wind_print_space(5);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        mutex = NODE_TO_MUTEX(dnode);
        wind_printf("%-16s %-8s %-16s \r\n",
            mutex->obj.name?mutex->obj.name:"null",IS_F_MUTEX_LOCKED(mutex)?"lock":"unlock",
            mutex->owner != W_NULL?mutex->owner->name:"null");
    }
    wind_enable_switch();
    wind_print_space(5);
    return W_ERR_OK;
}

#endif // #if WIND_MUTEX_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
