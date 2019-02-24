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
#include "wind_dlist.h"
#include "wind_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
#if WIND_MUTEX_SUPPORT
#define WIND_MUTEX_MAGIC 0x37AD490F

#define F_MUTEX_POOL (0x01 << 0) //标记mutex对象是否通过内存池分配
#define IS_F_MUTEX_POOL(mutex) ((mutex->flag & F_MUTEX_POOL) == F_MUTEX_POOL)
#define SET_F_MUTEX_POOL(mutex) (mutex->flag |= F_MUTEX_POOL)
#define CLR_F_MUTEX_POOL(mutex) (mutex->flag &= (~F_MUTEX_POOL))

#define F_MUTEX_LOCKED (0x01 << 1) //标记mutex对象当前的锁定状态
#define IS_F_MUTEX_LOCKED(mutex) ((mutex->flag & F_MUTEX_LOCKED) == F_MUTEX_LOCKED)
#define SET_F_MUTEX_LOCKED(mutex) (mutex->flag |= F_MUTEX_LOCKED)
#define CLR_F_MUTEX_LOCKED(mutex) (mutex->flag &= (~F_MUTEX_LOCKED))


typedef struct _w_mutex_s
{
    w_uint32_t magic;//魔术字
    const char *name;//互斥锁的名字
    w_dnode_s mutexnode;//互斥锁的节点，用于加入链表
    w_uint16_t flag;
    w_uint16_t nest;  //同一线程互斥锁嵌套层次
    w_thread_s *owner;//当前持有互斥锁的线程
    w_dlist_s waitlist;  //等待线程队列
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
w_err_t wind_mutex_print(void);

#endif

#ifdef __cplusplus
}
#endif


#endif

