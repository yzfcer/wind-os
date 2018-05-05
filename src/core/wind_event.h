/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_event.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的消息相关的代码头文件
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
#ifndef WIND_EVENT_H__
#define WIND_EVENT_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_EVENT_SUPPORT
typedef struct __event_s

#define WIND_EVENT_MAGIC 0x579A524C
typedef 
typedef struct _wind_event
{
    dnode_s listenernode;
    
}listener_s; 

typedef struct __event_s
{
    w_uint32_t magic;//魔术字
    const char *name;
    dnode_s eventnode;
    dlist_s msglist;//消息队列
    int msgnum;//消息的数量
    thread_s *owner;
}event_s;


#define GET_MSG(nodeptr,msgtype,msgnode) (void*)(((char*)(nodeptr))-((w_uint32_t)&(((msgtype*)0)->msgnode)))

void wind_msg_init(listener_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg);
w_err_t _wind_event_init(void);
event_s *wind_event_get(const char *name);
event_s *wind_event_create(const char *name);

w_err_t wind_event_trydestroy(event_s *event);
w_err_t wind_event_destroy(event_s *event);

w_err_t wind_event_trig(event_s *event,listener_s *pmsg);
w_err_t wind_event_wait(event_s *event,listener_s **pmsg,w_uint32_t timeout);
w_err_t wind_event_trywait(event_s *event,listener_s **pmsg);
w_err_t wind_event_print(dlist_s *list);

#else
#define _wind_event_init()
#endif //WIND_MSGBOX_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_MESSAGE_H__

