/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_coroutine.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os coroutine function module
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
#ifndef WIND_COROUTINE_H__
#define WIND_COROUTINE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_COROUTINE_SUPPORT
#define WIND_COROUTINE_MAGIC 0x67F52854

#define F_COROUTINE_POOL (0x01 << 0) //Mark whether the coroutine object is allocated through the memory pool
#define IS_F_COROUTINE_POOL(coroutine) ((coroutine->obj.userflag & F_COROUTINE_POOL) == F_COROUTINE_POOL)
#define SET_F_COROUTINE_POOL(coroutine) (coroutine->obj.userflag |= F_COROUTINE_POOL)
#define CLR_F_COROUTINE_POOL(coroutine) (coroutine->obj.userflag &= (~F_COROUTINE_POOL))

#define F_COROUTINE_READY (0x01 << 1) //Mark whether the coroutine object is ready
#define IS_F_COROUTINE_READY(coroutine) ((coroutine->obj.userflag & F_COROUTINE_READY) == F_COROUTINE_READY)
#define SET_F_COROUTINE_READY(coroutine) (coroutine->obj.userflag |= F_COROUTINE_READY)
#define CLR_F_COROUTINE_READY(coroutine) (coroutine->obj.userflag &= (~F_COROUTINE_READY))

typedef w_uint16_t w_cid_t;
typedef w_err_t (*coroutine_fn)(void *arg);

typedef struct _w_coroutine_s
{
    w_obj_s obj;       //Basic object information
    w_cid_t cid;       //Coroutine ID
    w_stack_t *stack;  //Coroutine stack pointer
    coroutine_fn func; //Coroutine function
    void *arg;         //Coroutine parameter
    w_stack_t stackbuff[WIND_COROUTINE_STKSIZE];//Coroutine stack buffer
}w_coroutine_s;

w_err_t _wind_coroutine_mod_init(void);
w_err_t _wind_coroutine_dispatch(void);
w_coroutine_s *wind_coroutine_get(const char *name);
w_err_t wind_coroutine_init(w_coroutine_s *coroutine,const char *name,w_uint16_t cid,coroutine_fn func,void *arg);
w_coroutine_s *wind_coroutine_create(const char *name,w_uint16_t cid,coroutine_fn func,void *arg);
w_err_t wind_coroutine_destroy(w_coroutine_s *coroutine);

w_err_t wind_coroutine_yeild(w_coroutine_s *coroutine,void *arg);
w_err_t wind_coroutine_resume(w_coroutine_s *coroutine,void *arg);

w_err_t wind_coroutine_print(void);

#endif // #if WIND_COROUTINE_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_COROUTINE_H__

