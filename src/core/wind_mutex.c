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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#include "wind_thread.h"
#include "wind_os_hwif.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_dlist.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_string.h"

#if WIND_MUTEX_SUPPORT
#define WIND_MUTEX_MAGIC 0x37AD490F
static WIND_POOL(mutexpool,WIND_MUTEX_MAX_NUM,sizeof(mutex_s));

static __INLINE__ mutex_s *mutex_malloc(void)
{
    return (mutex_s*)wind_pool_alloc(mutexpool);
}

static __INLINE__ w_err_t mutex_free(void *mutex)
{
    return wind_pool_free(mutexpool,mutex);
}
w_err_t _wind_mutex_init(void)
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
    wind_notice("create mutex:%s",name);
    mutex = mutex_malloc();
    WIND_ASSERT_TODO(mutex != NULL,wind_open_interrupt(),NULL);
    mutex->magic = WIND_MUTEX_MAGIC;
    DNODE_INIT(mutex->mutexnode);
    mutex->name = name;
    mutex->mutexed = B_FALSE;
    DLIST_INIT(mutex->waitlist);
    
    wind_close_interrupt();
    dlist_insert_tail(&g_core.mutexlist,&mutex->mutexnode);
    wind_open_interrupt();
    return mutex;
}

//试图释放一个互斥锁，如果有线程被阻塞，则释放将终止
w_err_t wind_mutex_try_destroy(mutex_s *mutex)
{
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed == B_FALSE,wind_open_interrupt(),ERR_COMMAN);
    wind_mutex_destroy(mutex);
    wind_open_interrupt();
    return ERR_OK;    
}

//强制性释放互斥锁，并把所有的被该互斥锁阻塞的线程全部激活
w_err_t wind_mutex_destroy(mutex_s *mutex)
{
    dnode_s *pnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_notice("destroy mutex:%s",mutex->name);
    wind_close_interrupt();
    foreach_node(pnode,&mutex->waitlist)
    {
        dlist_remove(&mutex->waitlist,pnode);
        thread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_LOCK;
    }
    dlist_remove(&g_core.mutexlist,&mutex->mutexnode);
    mutex->name = NULL;
    mutex->magic = 0;
    mutex_free(mutex);
    wind_open_interrupt();
    return ERR_OK;
}


w_err_t wind_mutex_lock(mutex_s *mutex)
{
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();

    if (mutex->mutexed == B_FALSE)
    {
        mutex->mutexed = B_TRUE;
        wind_open_interrupt();
        return ERR_OK; 
    }
    thread = wind_thread_current();
    thread->runstat = THREAD_STATUS_SUSPEND;
    thread->cause = CAUSE_LOCK;
    thread->sleep_ticks = 0x7fffffff;
    
    dlist_insert_prio(&mutex->waitlist,&thread->suspendthr,thread->prio);
    wind_open_interrupt();
    _wind_thread_dispatch();
    return ERR_OK;
}

w_err_t wind_mutex_trylock(mutex_s *mutex)
{
    w_err_t err;
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    if (mutex->mutexed == B_FALSE)
    {
        mutex->mutexed = B_TRUE;
        err = ERR_OK; 
    }
    else
        err = ERR_COMMAN; 
    wind_open_interrupt();
    return err;
}


//试图打开一个互斥锁，如果有线程被阻塞，则优先激活线程
w_err_t wind_mutex_unlock(mutex_s *mutex)
{
    dnode_s *pnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed,wind_open_interrupt(),ERR_OK);
    
    pnode = dlist_head(&mutex->waitlist);
    if (pnode == NULL)
    {
        mutex->mutexed = B_FALSE;
        wind_open_interrupt();
        return ERR_OK; //信号量有效，直接返回效，
    }

    dlist_remove_head(&mutex->waitlist);
    thread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_LOCK;
    wind_open_interrupt();
    return ERR_OK;    
}


w_err_t wind_mutex_print(dlist_s *list)
{
    dnode_s *dnode;
    mutex_s *mutex;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nmutex list as following:\r\n");
    wind_printf("--------------------------------------\r\n");
    wind_printf("%-16s %-8s\r\n","mutex","status");
    wind_printf("--------------------------------------\r\n");
    foreach_node(dnode,list)
    {
        mutex = (mutex_s *)DLIST_OBJ(dnode,mutex_s,mutexnode);
        wind_printf("%-16s %-8s\r\n",
            mutex->name,mutex->mutexed?"lock":"unlock");
    }
    wind_printf("--------------------------------------\r\n");
    return ERR_OK;
}

#endif

