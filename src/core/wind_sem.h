/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_sem.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os semaphore function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
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

w_err_t wind_sem_print_list(void);
w_err_t wind_sem_print_detail(void);

#endif // #if WIND_SEM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_SEM_H__

