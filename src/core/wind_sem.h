/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sem.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的信号量头文件
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
#ifndef WIND_SEM_H__
#define WIND_SEM_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_SEM_SUPPORT
#define WIND_SEM_MAGIC 0x36F7A854

#define F_SEM_POOL (0x01 << 0) //Mark whether the SEM object is allocated through the memory pool
#define IS_F_SEM_POOL(sem) ((sem->obj.flag & F_SEM_POOL) == F_SEM_POOL)
#define SET_F_SEM_POOL(sem) (sem->obj.flag |= F_SEM_POOL)
#define CLR_F_SEM_POOL(sem) (sem->obj.flag &= (~F_SEM_POOL))

typedef struct _w_sem_s
{
    w_obj_s obj;         //SEM basic object information
    w_dlist_s waitlist;  //List of threads waiting for semaphores
    w_int8_t sem_tot;    //Initial value of semaphore
    w_int8_t sem_num;    //Current value of semaphore
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

#endif // #if WIND_SEM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_SEM_H__

