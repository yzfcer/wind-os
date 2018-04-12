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
#include "wind_mpool.h"
static WIND_MPOOL(mutexpool,WIND_MUTEX_MAX_NUM,sizeof(mutex_s));

static __INLINE__ mutex_s *mutex_malloc(void)
{
    return (mutex_s*)wind_pool_alloc(mutexpool);
}

static __INLINE__ w_err_t mutex_free(void *mutex)
{
    return wind_pool_free(mutexpool,mutex);
}
w_err_t wind_mutex_init(void)
{
    w_err_t err;
    err = wind_pool_create("mutex",mutexpool,sizeof(mutexpool),sizeof(mutex_s));
    return err;
}

//����һ��mutex���󣬲���������mutex�б�
mutex_s *wind_mutex_create(const char *name)
{
    mutex_s *pmutex;
    wind_notice("create mutex:%s",name);
    pmutex = mutex_malloc();
    WIND_ASSERT_TODO(pmutex != NULL,wind_open_interrupt(),NULL);
    DNODE_INIT(pmutex->mutexnode);
    pmutex->name = name;
    pmutex->mutexed = B_FALSE;
    DLIST_INIT(pmutex->waitlist);
    
    wind_close_interrupt();
    dlist_insert_tail(&g_core.mutexlist,&pmutex->mutexnode);
    wind_open_interrupt();
    return pmutex;
}

//��ͼ�ͷ�һ����������������̱߳����������ͷŽ���ֹ
w_err_t wind_mutex_try_destroy(mutex_s *pmutex)
{
    WIND_ASSERT_RETURN(pmutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(pmutex->mutexed == B_FALSE,wind_open_interrupt(),ERR_COMMAN);
    wind_mutex_destroy(pmutex);
    wind_open_interrupt();
    return ERR_OK;    
}

//ǿ�����ͷŻ��������������еı��û������������߳�ȫ������
w_err_t wind_mutex_destroy(mutex_s *pmutex)
{
    dnode_s *pnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmutex != NULL,ERR_NULL_POINTER);
    wind_notice("destroy mutex:%s",pmutex->name);
    wind_close_interrupt();
    foreach_node(pnode,&pmutex->waitlist)
    {
        dlist_remove(&pmutex->waitlist,pnode);
        pthread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
        pthread->runstat = THREAD_STATUS_READY;
        pthread->cause = CAUSE_LOCK;
    }
    dlist_remove(&g_core.mutexlist,&pmutex->mutexnode);
    pmutex->name = NULL;
    mutex_free(pmutex);
    wind_open_interrupt();
    return ERR_OK;
}

//��ͼ����һ��������������Ѿ������������߳̽�������
w_err_t wind_mutex_lock(mutex_s *pmutex)
{
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();

    if (pmutex->mutexed == B_FALSE)
    {
        pmutex->mutexed = B_TRUE;
        wind_open_interrupt();
        return ERR_OK; 
    }
    pthread = wind_thread_current();
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_LOCK;
    pthread->sleep_ticks = 0x7fffffff;
    
    dlist_insert_prio(&pmutex->waitlist,&pthread->suspendthr,pthread->prio);
    wind_open_interrupt();
    wind_thread_dispatch();
    return ERR_OK;
}

//��ͼ��һ����������������̱߳������������ȼ����߳�
w_err_t wind_mutex_unlock(mutex_s *pmutex)
{
    dnode_s *pnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmutex != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(pmutex->mutexed,wind_open_interrupt(),ERR_OK);
    
    pnode = dlist_head(&pmutex->waitlist);
    if (pnode == NULL)
    {
        pmutex->mutexed = B_FALSE;
        wind_open_interrupt();
        return ERR_OK; //�ź�����Ч��ֱ�ӷ���Ч��
    }

    dlist_remove_head(&pmutex->waitlist);
    pthread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_LOCK;
    wind_open_interrupt();
    return ERR_OK;    
}


w_err_t wind_mutex_print(dlist_s *list)
{
    dnode_s *dnode;
    mutex_s *pmutex;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nmutex list as following:\r\n");
    wind_printf("--------------------------------------\r\n");
    wind_printf("%-16s %-8s\r\n","mutex","status");
    wind_printf("--------------------------------------\r\n");
    foreach_node(dnode,list)
    {
        pmutex = (mutex_s *)DLIST_OBJ(dnode,mutex_s,mutexnode);
        wind_printf("%-16s %-8s\r\n",
            pmutex->name,pmutex->mutexed?"lock":"unlock");
    }
    wind_printf("--------------------------------------\r\n");
    return ERR_OK;
}



