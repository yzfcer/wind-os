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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_sem.h"
#include "dlist.h"
#include "wind_err.h"
#include "wind_stati.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_assert.h"
#include "wind_string.h"
#include "core_obj.h"
#if WIND_SEM_SUPPORT
extern void wind_thread_dispatch(void);
static psem_s sem_malloc(void)
{
    psem_s psem;
    psem = wind_core_alloc(IDX_SEM);
    return psem;
}

static w_err_t sem_free(psem_s psem)
{
    return wind_core_free(IDX_SEM,psem);
}


psem_s wind_sem_create(const char *name,w_uint16_t sem_value)
{
    psem_s psem;
    psem = sem_malloc();
    WIND_ASSERT_RETURN(psem != NULL,NULL);
    wind_memset(psem->name,0,SEM_NAME_LEN);
    wind_strcpy(psem->name,name);
    DNODE_INIT(psem->semnode);
    psem->used = B_TRUE;
    psem->sem_num = sem_value;
    psem->sem_tot = sem_value;
    DLIST_INIT(psem->waitlist);
    wind_close_interrupt();
    dlist_insert_tail(&g_core.semlist,&psem->semnode);
    wind_open_interrupt();
    return psem;
}

w_err_t wind_sem_post(psem_s psem)
{
    pdnode_s pnode;
    pthread_s pthread;
    WIND_ASSERT_RETURN(psem != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    //无阻塞的线程，减少信号量后直接返回
    pnode = dlist_head(&psem->waitlist);
    if(pnode == NULL)
    {
        if(psem->sem_num < psem->sem_tot)
            psem->sem_num ++;
        wind_open_interrupt();
        return ERR_OK;
    }
    
    //激活被阻塞的线程，从睡眠队列移除,触发线程切换
    dlist_remove(&psem->waitlist,pnode);
    pthread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
    dlist_remove(&g_core.sleeplist,&pthread->sleepthr.node);
    wind_open_interrupt();
    pthread->cause = CAUSE_SEM;
    pthread->runstat = THREAD_STATUS_READY;
    wind_thread_dispatch();
    return ERR_OK;
}


w_err_t wind_sem_fetch(psem_s psem,w_uint32_t timeout)
{
    w_int32_t ticks;
    pthread_s pthread;
    WIND_ASSERT_RETURN(psem != NULL,ERR_NULL_POINTER);
    ticks = timeout * WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;

    //信号量有效，直接返回
    wind_close_interrupt();
    if (psem->sem_num > 0)
    {
        psem->sem_num --;
        wind_open_interrupt();
        return ERR_OK; 
    }

    //将当前线程加入睡眠和阻塞队列，触发线程切换
    pthread = wind_thread_current();
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_SEM;
    pthread->sleep_ticks = ticks;
    dlist_insert_prio(&g_core.sleeplist,&pthread->sleepthr,pthread->prio);
    dlist_insert_prio(&psem->waitlist,&pthread->sleepthr,pthread->prio);
    wind_open_interrupt();
    wind_thread_dispatch();

    //如果是被信号唤醒的
    wind_close_interrupt();
    if(pthread->cause == CAUSE_SEM)
    {
        
    }
    //如果是超时唤醒的，则移除出睡眠队列
    else if(pthread->cause == CAUSE_SLEEP)
    {
        dlist_remove(&psem->waitlist,&pthread->suspendthr.node);
    }
    wind_open_interrupt();
    if(pthread->cause == CAUSE_SLEEP)
        return ERR_TIMEOUT;
    return ERR_OK;
}



//试图释放一个信号量，如果有线程被阻塞，则释放将终止
w_err_t wind_sem_tryfree(psem_s psem)
{
    pdnode_s pdnode;
    WIND_ASSERT_RETURN(psem != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    pdnode = dlist_head(&psem->waitlist);
    if(pdnode != NULL)
    {
        wind_open_interrupt();
        return ERR_COMMAN;
    }
    wind_open_interrupt();
    return wind_sem_free(psem);
}

w_err_t wind_sem_free(psem_s psem)
{
    w_err_t err;
    pdnode_s pdnode;
    pthread_s pthread;
    WIND_ASSERT_RETURN(psem != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    foreach_node(pdnode,&psem->waitlist)
    {
        dlist_remove(&psem->waitlist,pdnode);
        pthread = PRI_DLIST_OBJ(pdnode,thread_s,suspendthr);
        pthread->runstat = THREAD_STATUS_READY;
        pthread->cause = CAUSE_SEM;
    }
    dlist_remove(&g_core.semlist,&psem->semnode);
    wind_open_interrupt();
    sem_free(psem);
    return err;    
}

w_err_t wind_sem_print(pdlist_s list)
{
    pdnode_s dnode;
    psem_s psem;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nsem list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s %-10s\r\n","sem","sem_tot","sem_num");
    wind_printf("----------------------------------------------\r\n");

    foreach_node(dnode,list)
    {
        psem = (psem_s)DLIST_OBJ(dnode,sem_s,semnode);
        wind_printf("%-16s %-8d %-10d\r\n",
            psem->name,psem->sem_tot,psem->sem_num);
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}

#endif

