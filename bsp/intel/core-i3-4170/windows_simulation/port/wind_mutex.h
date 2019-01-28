/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_mutex.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ϵͳ�Ļ�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.2
** �ա���: 2013.06.23
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.06.23
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LOCK_H__
#define WIND_LOCK_H__

#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
#if WIND_MUTEX_SUPPORT

typedef struct _w_mutex_s
{
    void* ptr;
}w_mutex_s;

#define _wind_mutex_mod_init() 0
#define wind_mutex_get(name) (void*)0
#define wind_mutex_init(mutex,name) 0
#define wind_mutex_create(name) (void*)0
#define wind_mutex_trydestroy(mutex) 0
#define wind_mutex_destroy(mutex) 0
#define wind_mutex_lock(mutex) 0
#define wind_mutex_trylock(mutex) 0
#define wind_mutex_unlock(mutex) 0
#define wind_mutex_print() 

#endif

#ifdef __cplusplus
}
#endif


#endif

