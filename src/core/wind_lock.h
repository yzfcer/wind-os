/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_lock.h
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

typedef struct _wind_lock
{
    dnode_s locknode;
    const char *name;
    w_bool_t locked;     //��ǰ���ź�����ֵ
    dlist_s waitlist;  //�ȴ��̶߳���
}lock_s;

//w_err_t wind_lock_init(void);
lock_s *wind_lock_create(const char *name);
w_err_t wind_lock_tryfree(lock_s *plock);
w_err_t wind_lock_free(lock_s *plock);
w_err_t wind_lock_close(lock_s *plock);
w_err_t wind_lock_open(lock_s *plock);
w_err_t wind_lock_print(dlist_s *list);


#ifdef __cplusplus
}
#endif


#endif

