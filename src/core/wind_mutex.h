/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_mutex.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 系统的互斥锁
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.2
** 日　期: 2013.06.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _wind_mutex
{
    dnode_s mutexnode;
    const char *name;
    w_bool_t mutexed;     //当前的信号量的值
    dlist_s waitlist;  //等待线程队列
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

