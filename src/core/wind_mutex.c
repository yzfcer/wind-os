/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mutex.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 系统的互斥锁
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.2
** 日　期: 2013.06.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.06.23
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_var.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_string.h"

#if WIND_MUTEX_SUPPORT
#define WIND_MUTEX_MAGIC 0x37AD490F
static WIND_POOL(mutexpool,WIND_MUTEX_MAX_NUM,sizeof(mutex_s));

static __INLINE__ mutex_s *mutex_malloc(void)
{
    return (mutex_s*)wind_pool_malloc(mutexpool);
}

static __INLINE__ w_err_t mutex_free(void *mutex)
{
    return wind_pool_free(mutexpool,mutex);
}
w_err_t _wind_mutex_mod_init(void)
{
    w_err_t err;
    err = wind_pool_create("mutex",mutexpool,sizeof(mutexpool),sizeof(mutex_s));
    return err;
}

mutex_s *wind_mutex_get(const char *name)
{
    mutex_s *mutex;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.mutexlist)
    {
        mutex = DLIST_OBJ(dnode,mutex_s,mutexnode);
        if(wind_strcmp(name,mutex->name) == 0)
        {
            wind_enable_switch();
            return mutex;
        }
    }
    wind_enable_switch();
    return NULL;
}

//创建一个mutex对象，并加入所有mutex列表
mutex_s *wind_mutex_create(const char *name)
{
    mutex_s *mutex;
    //wind_notice("create mutex:%s",name);
    mutex = mutex_malloc();
    WIND_ASSERT_TODO(mutex != NULL,wind_enable_interrupt(),NULL);
    mutex->magic = WIND_MUTEX_MAGIC;
    DNODE_INIT(mutex->mutexnode);
    mutex->name = name;
    mutex->mutexed = B_FALSE;
    mutex->nest = 0;
    mutex->owner = NULL;
    DLIST_INIT(mutex->waitlist);
    
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.mutexlist,&mutex->mutexnode);
    wind_enable_interrupt();
    return mutex;
}

//试图销毁一个互斥锁，如果有线程被阻塞，则销毁将终止
w_err_t wind_mutex_trydestroy(mutex_s *mutex)
{
    WIND_ASSERT_RETURN(mutex != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed == B_FALSE,wind_enable_interrupt(),W_ERR_FAIL);
    wind_mutex_destroy(mutex);
    wind_enable_interrupt();
    return W_ERR_OK;    
}

//强制性销毁互斥锁，并把所有的被该互斥锁阻塞的线程全部激活
w_err_t wind_mutex_destroy(mutex_s *mutex)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    //wind_notice("destroy mutex:%s",mutex->name);
    wind_disable_interrupt();
    dlist_remove(&g_core.mutexlist,&mutex->mutexnode);
    foreach_node(dnode,&mutex->waitlist)
    {
        dlist_remove(&mutex->waitlist,dnode);
        thread = PRI_DLIST_OBJ(dnode,thread_s,suspendnode);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_LOCK;
    }
    wind_enable_interrupt();
    mutex->magic = 0;
    mutex_free(mutex);
    return W_ERR_OK;
}


w_err_t wind_mutex_lock(mutex_s *mutex)
{
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    thread = wind_thread_current();

    if (mutex->mutexed == B_FALSE)
    {
        mutex->mutexed = B_TRUE;
        mutex->nest ++;
        mutex->owner = wind_thread_current();
        wind_enable_interrupt();
        return W_ERR_OK; 
    }
    if(thread == mutex->owner)
    {
        if(mutex->nest < 65535)
            mutex->nest ++;
        wind_enable_interrupt();
        return W_ERR_OK; 
    }
    
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_LOCK;
    thread->sleep_ticks = 0x7fffffff;
    
    dlist_insert_prio(&mutex->waitlist,&thread->suspendnode,thread->prio);
    wind_enable_interrupt();
    _wind_thread_dispatch();
    return W_ERR_OK;
}

w_err_t wind_mutex_trylock(mutex_s *mutex)
{
    w_err_t err;
    WIND_ASSERT_RETURN(mutex != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    if (mutex->mutexed == B_FALSE)
    {
        mutex->mutexed = B_TRUE;
        mutex->owner = wind_thread_current();
        err = W_ERR_OK; 
    }
    else
        err = W_ERR_FAIL; 
    wind_enable_interrupt();
    return err;
}


//试图打开一个互斥锁，如果有线程被阻塞，则优先激活线程
w_err_t wind_mutex_unlock(mutex_s *mutex)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,W_ERR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed,wind_enable_interrupt(),W_ERR_OK);
    thread = wind_thread_current();
    WIND_ASSERT_TODO(mutex->owner == thread,wind_enable_interrupt(),W_ERR_FAIL);
    if(mutex->nest > 0)
        mutex->nest --;
    if(mutex->nest > 0)
    {
        wind_enable_interrupt();
        return W_ERR_OK;
    }
    dnode = dlist_head(&mutex->waitlist);
    if (dnode == NULL)
    {
        mutex->mutexed = B_FALSE;
        mutex->owner = NULL;
        wind_enable_interrupt();
        return W_ERR_OK; //信号量有效，直接返回效，
    }

    dlist_remove_head(&mutex->waitlist);
    thread = PRI_DLIST_OBJ(dnode,thread_s,suspendnode);
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_LOCK;
    mutex->owner = thread;
    wind_enable_interrupt();
    return W_ERR_OK;    
}


w_err_t wind_mutex_print(dlist_s *list)
{
    dnode_s *dnode;
    mutex_s *mutex;
    WIND_ASSERT_RETURN(list != NULL,W_ERR_NULL);
    wind_printf("\r\n\r\nmutex list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s \r\n","mutex","status","owner");
    wind_print_space(5);
    foreach_node(dnode,list)
    {
        mutex = (mutex_s *)DLIST_OBJ(dnode,mutex_s,mutexnode);
        wind_printf("%-16s %-8s %-16s \r\n",
            mutex->name,mutex->mutexed?"lock":"unlock",
            mutex->owner != NULL?mutex->owner->name:"null");
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif

