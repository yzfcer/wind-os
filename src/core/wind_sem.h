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
#define WIND_SEM_MAGIC 0x36F7A854
typedef struct _w_sem_s
{
    w_uint32_t magic;
    const char* name;
    w_dnode_s semnode;
    w_dlist_s waitlist;  //�ȴ��̶߳���
    //w_dlist_s ownerlist;  //ӵ���ź����̶߳���
    w_int16_t sem_tot;    //��ʼ�����ź�����ֵ
    w_int16_t sem_num;    //��ǰ���ź�����ֵ
}w_sem_s;

w_err_t _wind_sem_mod_init(void);
w_sem_s *wind_sem_get(const char *name);
w_sem_s *wind_sem_create(const char *name,w_int16_t semValue);
w_err_t wind_sem_trydestroy(w_sem_s *sem);
w_err_t wind_sem_destroy(w_sem_s *sem);

w_err_t wind_sem_post(w_sem_s *sem);
w_err_t wind_sem_wait(w_sem_s *sem,w_uint32_t timeout);
w_err_t wind_sem_trywait(w_sem_s *sem);

w_err_t wind_sem_print(w_dlist_s *list);
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_SEM_H__

