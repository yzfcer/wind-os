/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_sem.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os���ź���ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_SEM_H__
#define WIND_SEM_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_SEM_SUPPORT
#define WIND_SEM_MAGIC 0x36F7A854

#define F_SEM_POOL (0x01 << 0) //���sem�����Ƿ�ͨ���ڴ�ط���
#define IS_F_SEM_POOL(sem) ((sem->obj.flag & F_SEM_POOL) == F_SEM_POOL)
#define SET_F_SEM_POOL(sem) (sem->obj.flag |= F_SEM_POOL)
#define CLR_F_SEM_POOL(sem) (sem->obj.flag &= (~F_SEM_POOL))

typedef struct _w_sem_s
{
    w_obj_s obj;
    w_dlist_s waitlist;  //�ȴ��̶߳���
    w_int8_t sem_tot;    //��ʼ�����ź�����ֵ
    w_int8_t sem_num;    //��ǰ���ź�����ֵ
}w_sem_s;

w_err_t _wind_sem_mod_init(void);
w_sem_s *wind_sem_get(const char *name);
w_err_t wind_sem_init(w_sem_s *sem,const char *name,w_int8_t sem_value);
w_sem_s *wind_sem_create(const char *name,w_int8_t sem_value);
w_err_t wind_sem_trydestroy(w_sem_s *sem);
w_err_t wind_sem_destroy(w_sem_s *sem);

w_err_t wind_sem_post(w_sem_s *sem);
w_err_t wind_sem_wait(w_sem_s *sem,w_uint32_t timeout);
w_err_t wind_sem_trywait(w_sem_s *sem);

w_err_t wind_sem_print(void);
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_SEM_H__

