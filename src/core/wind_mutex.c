/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_mutex.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: ϵͳ�Ļ�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.2
** �ա���: 2013.06.23
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.06.23
** �衡��: 
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

//����һ��mutex���󣬲���������mutex�б�
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

//��ͼ�ͷ�һ����������������̱߳����������ͷŽ���ֹ
w_err_t wind_mutex_try_destroy(mutex_s *mutex)
{
    WIND_ASSERT_RETURN(mutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed == B_FALSE,wind_open_interrupt(),ERR_COMMAN);
    wind_mutex_destroy(mutex);
    wind_open_interrupt();
    return ERR_OK;    
}

//ǿ�����ͷŻ��������������еı��û������������߳�ȫ������
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


//��ͼ��һ����������������̱߳������������ȼ����߳�
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
        return ERR_OK; //�ź�����Ч��ֱ�ӷ���Ч��
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

