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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _wind_mutex
{
    dnode_s mutexnode;
    const char *name;
    w_bool_t mutexed;     //��ǰ���ź�����ֵ
    dlist_s waitlist;  //�ȴ��̶߳���
}mutex_s;

w_err_t wind_mutex_init(void);
mutex_s *wind_mutex_create(const char *name);
w_err_t wind_mutex_try_destroy(mutex_s *pmutex);
w_err_t wind_mutex_destroy(mutex_s *pmutex);
w_err_t wind_mutex_lock(mutex_s *pmutex);
w_err_t wind_mutex_unlock(mutex_s *pmutex);
w_err_t wind_mutex_print(dlist_s *list);


#ifdef __cplusplus
}
#endif


#endif

