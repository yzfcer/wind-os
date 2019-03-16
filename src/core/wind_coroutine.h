/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_coroutine.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��Э�̹���ģ��
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
#ifndef WIND_COROUTINE_H__
#define WIND_COROUTINE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_COROUTINE_SUPPORT
#define WIND_COROUTINE_MAGIC 0x67F52854

#define F_COROUTINE_POOL (0x01 << 0) //���coroutine�����Ƿ�ͨ���ڴ�ط���
#define IS_F_COROUTINE_POOL(coroutine) ((coroutine->obj.flag & F_COROUTINE_POOL) == F_COROUTINE_POOL)
#define SET_F_COROUTINE_POOL(coroutine) (coroutine->obj.flag |= F_COROUTINE_POOL)
#define CLR_F_COROUTINE_POOL(coroutine) (coroutine->obj.flag &= (~F_COROUTINE_POOL))

#define F_COROUTINE_READY (0x01 << 1) //���coroutine�����Ƿ�ready
#define IS_F_COROUTINE_READY(coroutine) ((coroutine->obj.flag & F_COROUTINE_READY) == F_COROUTINE_READY)
#define SET_F_COROUTINE_READY(coroutine) (coroutine->obj.flag |= F_COROUTINE_READY)
#define CLR_F_COROUTINE_READY(coroutine) (coroutine->obj.flag &= (~F_COROUTINE_READY))

typedef w_uint16_t w_cid_t;
typedef w_err_t (*coroutine_fn)(void *arg);

typedef struct _w_coroutine_s
{
    w_obj_s obj;
    w_cid_t cid;
    w_thread_s *thread;
    w_stack_t *stack;
    coroutine_fn func;
    void *arg;
    w_stack_t stackbuff[WIND_COROUTINE_STKSIZE];
}w_coroutine_s;

w_err_t _wind_coroutine_mod_init(void);
w_coroutine_s *wind_coroutine_get(const char *name);
w_err_t wind_coroutine_init(w_coroutine_s *coroutine,const char *name,w_uint16_t cid,coroutine_fn func,void *arg);
w_coroutine_s *wind_coroutine_create(const char *name,w_uint16_t cid,coroutine_fn func,void *arg);
w_err_t wind_coroutine_destroy(w_coroutine_s *coroutine);

w_err_t wind_coroutine_yeild(w_coroutine_s *coroutine,void *arg);
w_err_t wind_coroutine_resume(w_coroutine_s *coroutine,void *arg);

w_err_t wind_coroutine_print(void);
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_COROUTINE_H__

