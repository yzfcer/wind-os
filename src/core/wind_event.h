/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_event.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���¼�������ع���
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
#define WIND_EVENT_MAGIC 0x5A9C524C
typedef struct __w_event_s w_event_s;
typedef void (*w_event_cb_fn)(w_event_s *event,void *arg);
	
typedef struct _w_event_cb
{
    w_dnode_s listenernode;
    w_event_cb_fn cb_fn;
}w_event_cb; 

struct __w_event_s
{
    w_uint32_t magic;//ħ����
    const char *name;
    w_dnode_s eventnode;
    w_dlist_s cblist;//��Ϣ����
    w_uint16_t flag_pool:1;
};


w_err_t _wind_event_mod_init(void);
w_event_s *wind_event_get(const char *name);
w_err_t wind_event_init(w_event_s *event,const char *name);
w_event_s *wind_event_create(const char *name);

w_err_t wind_event_destroy(w_event_s *event);

w_err_t wind_event_regcb(w_event_s *event,w_event_cb *cb);
w_err_t wind_event_unregcb(w_event_s *event,w_event_cb *cb);
w_err_t wind_event_trig(w_event_s *event,void *arg);
w_err_t wind_event_print(w_dlist_s *list);


#endif //WIND_EVENT_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_EVENT_H__

