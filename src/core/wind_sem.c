/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_sem.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os semaphore function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_sem.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_SEM_SUPPORT

#define NODE_TO_SEM(node) NODEPTR_TO_ITEMPTR(node,w_sem_s,obj.objnode)
static w_dlist_s semlist;
static WIND_POOL(sempool,WIND_SEM_MAX_NUM,sizeof(w_sem_s));

static __INLINE__ w_sem_s *sem_malloc(void)
{
    return (w_sem_s*)wind_pool_malloc(sempool);
}

static __INLINE__ w_err_t sem_free(void *sem)
{
    return wind_pool_free(sempool,sem);
}


w_err_t _wind_sem_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(semlist);
    err = wind_pool_create("sem",sempool,sizeof(sempool),sizeof(w_sem_s));
    return err;
}

w_sem_s *wind_sem_get(const char *name)
{
	return (w_sem_s*)wind_obj_get(name,&semlist);
}

w_err_t wind_sem_init(w_sem_s *sem,const char *name,w_int8_t sem_value)
{
    wind_notice("init sem:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sem_value >= 0,W_ERR_INVALID);
    sem->sem_num = sem_value;
    sem->sem_tot = sem_value;
    DLIST_INIT(sem->waitlist);
    wind_obj_init(&sem->obj,WIND_SEM_MAGIC,name,&semlist);
    CLR_F_SEM_POOL(sem);
    return W_ERR_OK;
}

w_sem_s *wind_sem_create(const char *name,w_int8_t sem_value)
{
    w_err_t err;
    w_sem_s *sem;
    sem = sem_malloc();
    WIND_ASSERT_RETURN(sem != W_NULL,W_NULL);
    err = wind_sem_init(sem,name,sem_value);
    if(err == W_ERR_OK)
    {
        SET_F_SEM_POOL(sem);
        return sem;
    }
    sem_free(sem);
    return W_NULL;
}


w_err_t wind_sem_trydestroy(w_sem_s *sem)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sem->obj.magic == WIND_SEM_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    dnode = dlist_head(&sem->waitlist);
    if(dnode != W_NULL)
    {
        wind_enable_switch();
        return W_ERR_FAIL;
    }
    wind_enable_switch();
    return wind_sem_destroy(sem);
}

w_err_t wind_sem_destroy(w_sem_s *sem)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    wind_notice("destroy sem:%s",wind_obj_name(&sem->obj));
    err = wind_obj_deinit(&sem->obj,WIND_SEM_MAGIC,&semlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    wind_disable_switch();
    foreach_node(dnode,&sem->waitlist)
    {
        dlist_remove(&sem->waitlist,dnode);
        thread = PRIDNODE_TO_THREAD(dnode,suspendnode);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_SEM;
    }
    wind_enable_switch();
    if(IS_F_SEM_POOL(sem))
        sem_free(sem);
    return W_ERR_OK;    
}

w_err_t wind_sem_post(w_sem_s *sem)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sem->obj.magic == WIND_SEM_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    
    dnode = dlist_head(&sem->waitlist);
    if(dnode == W_NULL)
    {
        if(sem->sem_num < sem->sem_tot)
            sem->sem_num ++;
        wind_enable_switch();
        return W_ERR_OK;
    }
    
    
    dlist_remove(&sem->waitlist,dnode);
    thread = PRIDNODE_TO_THREAD(dnode,suspendnode);
    if(thread->runstat == THREAD_STATUS_SLEEP)
        dlist_remove(sleeplist,&thread->sleepnode.dnode);
    wind_enable_switch();
    thread->cause = CAUSE_SEM;
    thread->runstat = THREAD_STATUS_READY;
    _wind_thread_dispatch();
    return W_ERR_OK;
}


w_err_t wind_sem_wait(w_sem_s *sem,w_uint32_t timeout)
{
    w_int32_t ticks;
    w_thread_s *thread;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sem->obj.magic == WIND_SEM_MAGIC,W_ERR_INVALID);
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;

    
    wind_disable_switch();
    if (sem->sem_num > 0)
    {
        sem->sem_num --;
        wind_enable_switch();
        return W_ERR_OK; 
    }
    if(timeout == 0)
        return W_ERR_FAIL;

    
    thread = wind_thread_current();
    thread->cause = CAUSE_SEM;
    if(timeout != SLEEP_TIMEOUT_MAX)
    {
        thread->runstat = THREAD_STATUS_SLEEP;
        thread->sleep_ticks = ticks;
        dlist_insert_prio(sleeplist,&thread->sleepnode,thread->prio);
    }
    else
        thread->runstat = THREAD_STATUS_SUSPEND;
    dlist_insert_prio(&sem->waitlist,&thread->suspendnode,thread->prio);
    wind_enable_switch();
    _wind_thread_dispatch();

    wind_disable_switch();
    
    if(thread->cause == CAUSE_SLEEP)
        dlist_remove(&sem->waitlist,&thread->suspendnode.dnode);
    wind_enable_switch();
    if(thread->cause == CAUSE_SLEEP)
        return W_ERR_TIMEOUT;
    return W_ERR_OK;
}

w_err_t wind_sem_trywait(w_sem_s *sem)
{
    w_err_t err;
    WIND_ASSERT_RETURN(sem != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(sem->obj.magic == WIND_SEM_MAGIC,W_ERR_INVALID);

    wind_disable_switch();
    if (sem->sem_num > 0)
    {
        sem->sem_num --;
        err = W_ERR_OK; 
    }
    else
        err = W_ERR_FAIL;
    wind_enable_switch();
    return err;
}

w_err_t wind_sem_print_list(void)
{
    return wind_obj_print_list(&semlist);
}

w_err_t wind_sem_print_detail(void)
{
    w_dnode_s *dnode;
    w_sem_s *sem;
    w_dlist_s *list = &semlist;
    wind_printf("\r\n\r\nsem list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-10s\r\n","sem","sem_tot","sem_num");
    wind_print_space(5);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        sem = NODE_TO_SEM(dnode);
        wind_printf("%-16s %-8d %-10d\r\n",
            sem->obj.name,sem->sem_tot,sem->sem_num);
    }
    wind_enable_switch();
    wind_print_space(5);
    return W_ERR_OK;
}

#endif // #if WIND_SEM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

