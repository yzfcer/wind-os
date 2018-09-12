/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sem.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的信号量头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
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
    w_dlist_s waitlist;  //等待线程队列
    //w_dlist_s ownerlist;  //拥有信号量线程队列
    w_int16_t sem_tot;    //初始化的信号量的值
    w_int16_t sem_num;    //当前的信号量的值
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

