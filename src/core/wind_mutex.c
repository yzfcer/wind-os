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
#include "wind_mutex.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_var.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_string.h"

#if WIND_MUTEX_SUPPORT
#define WIND_MUTEX_MAGIC 0x37AD490F
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
    err = wind_pool_create("mutex",mutexpool,sizeof(mutexpool),sizeof(w_mutex_s));
    return err;
}

w_mutex_s *wind_mutex_get(const char *name)
{
    w_mutex_s *mutex;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.mutexlist)
    {
        mutex = DLIST_OBJ(dnode,w_mutex_s,mutexnode);
        if(wind_strcmp(name,mutex->name) == 0)
        {
            wind_enable_switch();
            return mutex;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

//����һ��mutex���󣬲���������mutex�б�
w_mutex_s *wind_mutex_create(const char *name)
{
    w_mutex_s *mutex;
    //wind_notice("create mutex:%s",name);
    mutex = mutex_malloc();
    WIND_ASSERT_TODO(mutex != W_NULL,wind_enable_interrupt(),W_NULL);
    mutex->magic = WIND_MUTEX_MAGIC;
    DNODE_INIT(mutex->mutexnode);
    mutex->name = name;
    mutex->mutexed = W_FALSE;
    mutex->nest = 0;
    mutex->owner = W_NULL;
    DLIST_INIT(mutex->waitlist);
    
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.mutexlist,&mutex->mutexnode);
    wind_enable_interrupt();
    return mutex;
}

//��ͼ����һ����������������̱߳������������ٽ���ֹ
w_err_t wind_mutex_trydestroy(w_mutex_s *mutex)
{
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    WIND_ASSERT_TODO(mutex->mutexed == W_FALSE,wind_enable_interrupt(),W_ERR_FAIL);
    wind_mutex_destroy(mutex);
    wind_enable_interrupt();
    return W_ERR_OK;    
}

//ǿ�������ٻ��������������еı��û������������߳�ȫ������
w_err_t wind_mutex_destroy(w_mutex_s *mutex)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    //wind_notice("destroy mutex:%s",mutex->name);
    wind_disable_interrupt();
    dlist_remove(&g_core.mutexlist,&mutex->mutexnode);
    foreach_node(dnode,&mutex->waitlist)
    {
        dlist_remove(&mutex->waitlist,dnode);
        thread = PRI_DLIST_OBJ(dnode,w_thread_s,suspendnode);
        thread->runstat = THREAD_STATUS_READY;
        thread->cause = CAUSE_LOCK;
    }
    wind_enable_interrupt();
    mutex->magic = 0;
    mutex_free(mutex);
    return W_ERR_OK;
}


w_err_t wind_mutex_lock(w_mutex_s *mutex)
{
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    thread = wind_thread_current();

    if (mutex->mutexed == W_FALSE)
    {
        mutex->mutexed = W_TRUE;
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

w_err_t wind_mutex_trylock(w_mutex_s *mutex)
{
    w_err_t err;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(mutex->magic == WIND_MUTEX_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    if (mutex->mutexed == W_FALSE)
    {
        mutex->mutexed = W_TRUE;
        mutex->owner = wind_thread_current();
        err = W_ERR_OK; 
    }
    else
        err = W_ERR_FAIL; 
    wind_enable_interrupt();
    return err;
}


//��ͼ��һ����������������̱߳������������ȼ����߳�
w_err_t wind_mutex_unlock(w_mutex_s *mutex)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(mutex != W_NULL,W_ERR_PTR_NULL);
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
    if (dnode == W_NULL)
    {
        mutex->mutexed = W_FALSE;
        mutex->owner = W_NULL;
        wind_enable_interrupt();
        return W_ERR_OK; //�ź�����Ч��ֱ�ӷ���Ч��
    }

    dlist_remove_head(&mutex->waitlist);
    thread = PRI_DLIST_OBJ(dnode,w_thread_s,suspendnode);
    thread->runstat = THREAD_STATUS_READY;
    thread->cause = CAUSE_LOCK;
    mutex->owner = thread;
    wind_enable_interrupt();
    return W_ERR_OK;    
}


w_err_t wind_mutex_print(w_dlist_s *list)
{
    w_dnode_s *dnode;
    w_mutex_s *mutex;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nmutex list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s \r\n","mutex","status","owner");
    wind_print_space(5);
    foreach_node(dnode,list)
    {
        mutex = (w_mutex_s *)DLIST_OBJ(dnode,w_mutex_s,mutexnode);
        wind_printf("%-16s %-8s %-16s \r\n",
            mutex->name,mutex->mutexed?"lock":"unlock",
            mutex->owner != W_NULL?mutex->owner->name:"null");
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif

