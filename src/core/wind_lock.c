/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_lock.c
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
#include "wind_lock.h"
#include "wind_thread.h"
#include "wind_os_hwif.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_dlist.h"
#include "wind_core.h"
#include "core_obj.h"
static __INLINE__ lock_s *lock_malloc(void)
{
    return wind_core_alloc(IDX_LOCK);
}

static __INLINE__ w_err_t lock_free(void *lock)
{
    return wind_core_free(IDX_LOCK,lock);
}

//����һ��lock���󣬲���������lock�б�
lock_s *wind_lock_create(const char *name)
{
    lock_s *plock;
    plock = lock_malloc();
    WIND_ASSERT_TODO(plock != NULL,wind_open_interrupt(),NULL);
    DNODE_INIT(plock->locknode);
    plock->name = name;
    plock->locked = B_FALSE;
    DLIST_INIT(plock->waitlist);
    
    wind_close_interrupt();
    dlist_insert_tail(&g_core.locklist,&plock->locknode);
    wind_open_interrupt();
    return plock;
}

//��ͼ�ͷ�һ����������������̱߳����������ͷŽ���ֹ
w_err_t wind_lock_tryfree(lock_s *plock)
{
    WIND_ASSERT_RETURN(plock != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(plock->locked == B_FALSE,wind_open_interrupt(),ERR_COMMAN);
    wind_lock_free(plock);
    wind_open_interrupt();
    return ERR_OK;    
}

//ǿ�����ͷŻ��������������еı��û������������߳�ȫ������
w_err_t wind_lock_free(lock_s *plock)
{
    dnode_s *pnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(plock != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    foreach_node(pnode,&plock->waitlist)
    {
        dlist_remove(&plock->waitlist,pnode);
        pthread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
        pthread->runstat = THREAD_STATUS_READY;
        pthread->cause = CAUSE_LOCK;
    }
    dlist_remove(&g_core.locklist,&plock->locknode);
    plock->name = NULL;
    lock_free(plock);
    wind_open_interrupt();
    return ERR_OK;
}

//��ͼ����һ��������������Ѿ������������߳̽�������
w_err_t wind_lock_close(lock_s *plock)
{
    thread_s *pthread;
    WIND_ASSERT_RETURN(plock != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();

    if (plock->locked == B_FALSE)
    {
        plock->locked = B_TRUE;
        wind_open_interrupt();
        return ERR_OK; 
    }
    pthread = wind_thread_current();
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_LOCK;
    pthread->sleep_ticks = 0x7fffffff;
    
    dlist_insert_prio(&plock->waitlist,&pthread->suspendthr,pthread->prio);
    wind_open_interrupt();
    wind_thread_dispatch();
    return ERR_OK;
}

//��ͼ��һ����������������̱߳������������ȼ����߳�
w_err_t wind_lock_open(lock_s *plock)
{
    dnode_s *pnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(plock != NULL,ERR_NULL_POINTER);
    wind_close_interrupt();
    WIND_ASSERT_TODO(plock->locked,wind_open_interrupt(),ERR_OK);
    
    pnode = dlist_head(&plock->waitlist);
    if (pnode == NULL)
    {
        plock->locked = B_FALSE;
        wind_open_interrupt();
        return ERR_OK; //�ź�����Ч��ֱ�ӷ���Ч��
    }

    dlist_remove_head(&plock->waitlist);
    pthread = PRI_DLIST_OBJ(pnode,thread_s,suspendthr);
    pthread->runstat = THREAD_STATUS_READY;
    pthread->cause = CAUSE_LOCK;
    wind_open_interrupt();
    return ERR_OK;    
}


w_err_t wind_lock_print(dlist_s *list)
{
    dnode_s *dnode;
    lock_s *plock;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(list->head != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nlock list as following:\r\n");
    wind_printf("--------------------------------------\r\n");
    wind_printf("%-16s %-8s\r\n","lock","status");
    wind_printf("--------------------------------------\r\n");
    foreach_node(dnode,list)
    {
        plock = (lock_s *)DLIST_OBJ(dnode,lock_s,locknode);
        wind_printf("%-16s %-8s\r\n",
            plock->name,plock->locked?"lock":"unlock");
    }
    wind_printf("--------------------------------------\r\n");
    return ERR_OK;
}



