/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_coroutine.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的协程功能模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
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

#define F_COROUTINE_POOL (0x01 << 0) //标记coroutine对象是否通过内存池分配
#define IS_F_COROUTINE_POOL(coroutine) ((coroutine->obj.flag & F_COROUTINE_POOL) == F_COROUTINE_POOL)
#define SET_F_COROUTINE_POOL(coroutine) (coroutine->obj.flag |= F_COROUTINE_POOL)
#define CLR_F_COROUTINE_POOL(coroutine) (coroutine->obj.flag &= (~F_COROUTINE_POOL))

#define F_COROUTINE_READY (0x01 << 1) //标记coroutine对象是否ready
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

