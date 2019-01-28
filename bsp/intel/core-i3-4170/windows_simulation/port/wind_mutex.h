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

