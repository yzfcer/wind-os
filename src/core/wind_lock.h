/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_lock.h
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

typedef struct _wind_lock
{
    dnode_s locknode;
    const char *name;
    w_bool_t used;         //是否有效的标志
    w_bool_t locked;     //当前的信号量的值
    dlist_s waitlist;  //等待线程队列
}lock_s,*plock_s;

//w_err_t wind_lock_init(void);
plock_s wind_lock_create(const char *name);
w_err_t wind_lock_tryfree(plock_s plock);
w_err_t wind_lock_free(plock_s plock);
w_err_t wind_lock_close(plock_s plock);
w_err_t wind_lock_open(plock_s plock);
w_err_t wind_lock_print(pdlist_s list);


#ifdef __cplusplus
}
#endif


#endif

