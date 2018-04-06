/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_sem.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���ź���ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_SEM_H__
#define WIND_SEM_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_SEM_SUPPORT

typedef struct _wind_sem
{
    const char* name;
    dnode_s semnode;
    dlist_s waitlist;  //�ȴ��̶߳���
    w_int16_t sem_tot;    //��ʼ�����ź�����ֵ
    w_int16_t sem_num;    //��ǰ���ź�����ֵ
}sem_s;

w_err_t wind_sem_init(void);
sem_s *wind_sem_create(const char *name,w_int16_t semValue);
w_err_t wind_sem_post(sem_s *psem);
w_err_t wind_sem_fetch(sem_s *psem,w_uint32_t timeout);
w_err_t wind_sem_tryfree(sem_s *psem);
w_err_t wind_sem_free(sem_s *psem);
w_err_t wind_sem_print(dlist_s *list);
#else
#define wind_sem_init() ERR_OK
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_SEM_H__

