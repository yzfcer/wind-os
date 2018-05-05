/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_event.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os����Ϣ��صĴ���ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
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
    w_uint32_t magic;//ħ����
    const char *name;
    dnode_s eventnode;
    dlist_s msglist;//��Ϣ����
    int msgnum;//��Ϣ������
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

