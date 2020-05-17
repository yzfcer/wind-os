/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mutex.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 系统的互斥锁
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.2
** 日　期: 2013.06.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.06.23
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LOCK_H__
#define WIND_LOCK_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_MUTEX_SUPPORT
#define WIND_MUTEX_MAGIC 0x37AD490F

#define F_MUTEX_POOL (0x01 << 0) //Mark whether mutex objects are allocated through memory pool
#define IS_F_MUTEX_POOL(mutex) ((mutex->obj.flag & F_MUTEX_POOL) == F_MUTEX_POOL)
#define SET_F_MUTEX_POOL(mutex) (mutex->obj.flag |= F_MUTEX_POOL)
#define CLR_F_MUTEX_POOL(mutex) (mutex->obj.flag &= (~F_MUTEX_POOL))

#define F_MUTEX_LOCKED (0x01 << 1) //Mark the current locking state of mutex object
#define IS_F_MUTEX_LOCKED(mutex) ((mutex->obj.flag & F_MUTEX_LOCKED) == F_MUTEX_LOCKED)
#define SET_F_MUTEX_LOCKED(mutex) (mutex->obj.flag |= F_MUTEX_LOCKED)
#define CLR_F_MUTEX_LOCKED(mutex) (mutex->obj.flag &= (~F_MUTEX_LOCKED))

//Mutex object
typedef struct _w_mutex_s
{
    w_obj_s obj;       //Basic object information
    w_uint16_t nest;   //Nesting level of mutex in the same thread
    w_thread_s *owner; //Thread that holding mutexes
    w_dlist_s waitlist;//Waiting thread list
}w_mutex_s;


w_err_t _wind_mutex_mod_init(void);
w_mutex_s *wind_mutex_get(const char *name);
w_err_t wind_mutex_init(w_mutex_s *mutex,const char *name);
w_mutex_s *wind_mutex_create(const char *name);
w_err_t wind_mutex_trydestroy(w_mutex_s *mutex);
w_err_t wind_mutex_destroy(w_mutex_s *mutex);
w_err_t wind_mutex_lock(w_mutex_s *mutex);
w_err_t wind_mutex_trylock(w_mutex_s *mutex);
w_err_t wind_mutex_unlock(w_mutex_s *mutex);
w_err_t wind_mutex_print_detail(void);

#endif // #if WIND_MUTEX_SUPPORT

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef WIND_LOCK_H__

