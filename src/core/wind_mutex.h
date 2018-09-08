/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_mutex.h
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
#ifndef WIND_LOCK_H__
#define WIND_LOCK_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
#if WIND_MUTEX_SUPPORT

typedef struct _wind_mutex
{
    w_uint32_t magic;
    const char *name;
    dnode_s mutexnode;
    w_bool_t mutexed;     //��ǰ���ź�����ֵ
    w_uint16_t nest;
    thread_s *owner;
    dlist_s waitlist;  //�ȴ��̶߳���
}mutex_s;

w_err_t _wind_mutex_mod_init(void);
mutex_s *wind_mutex_get(const char *name);
mutex_s *wind_mutex_create(const char *name);
w_err_t wind_mutex_trydestroy(mutex_s *mutex);
w_err_t wind_mutex_destroy(mutex_s *mutex);
w_err_t wind_mutex_lock(mutex_s *mutex);
w_err_t wind_mutex_trylock(mutex_s *mutex);
w_err_t wind_mutex_unlock(mutex_s *mutex);
w_err_t wind_mutex_print(dlist_s *list);

#endif

#ifdef __cplusplus
}
#endif


#endif

