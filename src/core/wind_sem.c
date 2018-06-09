/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sem.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的信号量头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_sem.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_SEM_SUPPORT
extern void _wind_thread_dispatch(void);
static WIND_POOL(sempool,WIND_SEM_MAX_NUM,sizeof(sem_s));

static __INLINE__ sem_s *sem_malloc(void)
{
    return (sem_s*)wind_pool_malloc(sempool);
}

static __INLINE__ w_err_t sem_free(void *sem)
{
    return wind_pool_free(sempool,sem);
}


w_err_t _wind_sem_init(void)
{
    w_err_t err;
    err = wind_pool_create("sem",sempool,sizeof(sempool),sizeof(sem_s));
    return err;
}

sem_s *wind_sem_get(const char *name)
{
    sem_s *sem;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.semlist)
    {
        sem = DLIST_OBJ(dnode,sem_s,semnode);
        if(wind_strcmp(name,sem->name) == 0)
        {
            wind_enable_switch();
            return sem;
        }
    }
    wind_enable_switch();
    return NULL;
}

sem_s *wind_sem_create(const char *name,w_int16_t sem_value)
{
    sem_s *sem;
    wind_notice("create sem:%s",name);
    sem = sem_malloc();
    WIND_ASSERT_RETURN(sem != NULL,NULL);
    WIND_ASSERT_RETURN(sem_value >= 0,NULL);
    sem->magic = WIND_SEM_MAGIC;
    sem->name = name;
    DNODE_INIT(sem->semnode);
    sem->sem_num = sem_value;
    sem->sem_tot = sem_value;
    DLIST_INIT(sem->waitlist);
    //DLIST_INIT(sem->ownerlist);
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.semlist,&sem->semnode);
    wind_enable_interrupt();
    return sem;
}

//试图销毁一个信号量，如果有线程被阻塞，则释放将终止
w_err_t wind_sem_trydestroy(sem_s *sem)
{
    dnode_s *pdnode;
    WIND_ASSERT_RETURN(sem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(sem->magic == WIND_SEM_MAGIC,ERR_INVALID_PARAM);
    wind_disable_interrupt();
    pdnode = dlist_head(&sem->waitlist);
    if(pdnode != NULL)
    {
        wind_enable_interrupt();
        return ERR_FAIL;
    }
    wind_enable_interrupt();
    return wind_sem_destroy(sem);
}

w_err_t wind_sem_destroy(sem_s *sem)
{
    w_err_t err;
    dnode_s *pdnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(sem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(sem->magic == WIND_SEM_MAGIC,ERR_INVALID_PARAM);
    wind_notice("destroy sem:%s",sem->name);
    wind_disable_interrupt();
    dlist_remove(&g_core.semlist,&sem->semnode);
    sem->magic = 0;
    foreach_node(pdnode,&sem->waitlist)
    {
        dlist_remove(&sem->waitlist,pdnode);
        thread = PRI_DLIST_OBJ(pdnode,thread_s,suspendnode);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_SEM;
    }
    wind_enable_interrupt();
    err = sem_free(sem);
    return err;    
}

w_err_t wind_sem_post(sem_s *sem)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(sem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(sem->magic == WIND_SEM_MAGIC,ERR_INVALID_PARAM);
    wind_disable_interrupt();
    //无阻塞的线程，减少信号量后直接返回
    dnode = dlist_head(&sem->waitlist);
    if(dnode == NULL)
    {
        if(sem->sem_num < sem->sem_tot)
            sem->sem_num ++;
        wind_enable_interrupt();
        return ERR_OK;
    }
    
    //激活被阻塞的线程，从睡眠队列移除,触发线程切换
    dlist_remove(&sem->waitlist,dnode);
    thread = PRI_DLIST_OBJ(dnode,thread_s,suspendnode);
    if(thread->runstat == THREAD_STATUS_SLEEP)
        dlist_remove(&g_core.sleeplist,&thread->sleepnode.dnode);
    wind_enable_interrupt();
    thread->cause = CAUSE_SEM;
    thread->runstat = THREAD_STATUS_READY;
    _wind_thread_dispatch();
    return ERR_OK;
}


w_err_t wind_sem_wait(sem_s *sem,w_uint32_t timeout)
{
    w_int32_t ticks;
    thread_s *thread;
    WIND_ASSERT_RETURN(sem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(sem->magic == WIND_SEM_MAGIC,ERR_INVALID_PARAM);
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;

    //信号量有效，直接返回
    wind_disable_interrupt();
    if (sem->sem_num > 0)
    {
        sem->sem_num --;
        wind_enable_interrupt();
        return ERR_OK; 
    }
    if(timeout == 0)
        return ERR_FAIL;

    //将当前线程加入睡眠和阻塞队列，触发线程切换
    thread = wind_thread_current();
    thread->cause = CAUSE_SEM;
    if(timeout != SLEEP_TIMEOUT_MAX)
    {
        thread->runstat = THREAD_STATUS_SLEEP;
        thread->sleep_ticks = ticks;
        dlist_insert_prio(&g_core.sleeplist,&thread->sleepnode,thread->prio);
    }
    else
        thread->runstat = THREAD_STATUS_SUSPEND;
    dlist_insert_prio(&sem->waitlist,&thread->suspendnode,thread->prio);
    wind_enable_interrupt();
    _wind_thread_dispatch();

    wind_disable_interrupt();
    //如果是超时唤醒的，则移除出睡眠队列
    if(thread->cause == CAUSE_SLEEP)
        dlist_remove(&sem->waitlist,&thread->suspendnode.dnode);
    wind_enable_interrupt();
    if(thread->cause == CAUSE_SLEEP)
        return ERR_TIMEOUT;
    return ERR_OK;
}

w_err_t wind_sem_trywait(sem_s *sem)
{
    w_err_t err;
    WIND_ASSERT_RETURN(sem != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(sem->magic == WIND_SEM_MAGIC,ERR_INVALID_PARAM);

    //信号量有效，直接返回
    wind_disable_interrupt();
    if (sem->sem_num > 0)
    {
        sem->sem_num --;
        err = ERR_OK; 
    }
    else
        err = ERR_FAIL;
    wind_enable_interrupt();
    return err;
}


w_err_t wind_sem_print(dlist_s *list)
{
    dnode_s *dnode;
    sem_s *sem;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nsem list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-10s\r\n","sem","sem_tot","sem_num");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        sem = (sem_s *)DLIST_OBJ(dnode,sem_s,semnode);
        wind_printf("%-16s %-8d %-10d\r\n",
            sem->name,sem->sem_tot,sem->sem_num);
    }
    wind_print_space(5);
    return ERR_OK;
}

#endif

