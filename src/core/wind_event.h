/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_event.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind os的事件对象相关功能
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
#ifndef WIND_EVENT_H__
#define WIND_EVENT_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"
#include "wind_mutex.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_EVENT_SUPPORT
#define WIND_EVENT_MAGIC 0x5A9C524C

#define F_EVENT_POOL (0x01 << 0) //Mark whether the event object is allocated through the memory pool
#define IS_F_EVENT_POOL(event) ((event->obj.flag & F_EVENT_POOL) == F_EVENT_POOL)
#define SET_F_EVENT_POOL(event) (event->obj.flag |= F_EVENT_POOL)
#define CLR_F_EVENT_POOL(event) (event->obj.flag &= (~F_EVENT_POOL))

typedef struct __w_event_s w_event_s;
typedef void (*w_event_cb_fn)(w_event_s *event,void *arg);


typedef struct _w_event_cb
{
    w_dnode_s listenernode;//Listening to linked list node
    w_event_cb_fn cb_fn;   //Listening callback function
}w_event_cb; 

struct __w_event_s
{
    w_obj_s obj;     //Basic object information
    w_int16_t cbcnt; //Number of callback functions
    w_dlist_s cblist;//List of callback functions
    w_mutex_s lock;
};

w_err_t _wind_event_mod_init(void);
w_event_s *wind_event_get(const char *name);
w_err_t wind_event_init(w_event_s *event,const char *name);
w_event_s *wind_event_create(const char *name);

w_err_t wind_event_destroy(w_event_s *event);

w_err_t wind_event_regcb(w_event_s *event,w_event_cb *cb);
w_err_t wind_event_unregcb(w_event_s *event,w_event_cb *cb);
w_err_t wind_event_trig(w_event_s *event,void *arg);
w_err_t wind_event_print_list(void);
w_err_t wind_event_print_detail(void);


#endif // #if WIND_EVENT_SUPPORT

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif  //WIND_EVENT_H__

