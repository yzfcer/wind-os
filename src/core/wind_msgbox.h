/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_message.h
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
#ifndef WIND_MESSAGE_H__
#define WIND_MESSAGE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_MESSAGE_SUPPORT

typedef struct _wind_message
{
    dnode_s msgnode;
    w_uint16_t msg_id;
    w_uint16_t msg_len;
    void *msg_arg;
}msg_s; 

typedef struct __mbox_s
{
    const char *name;
    dnode_s mboxnode;
    dlist_s msglist;//��Ϣ����
    int num;//��Ϣ������
    w_bool_t valid;//�����Ƿ����
    thread_s *owner;
}mbox_s;


#define GET_MSG(nodeptr,msgtype,msgnode) (void*)(((char*)(nodeptr))-((w_uint32_t)&(((msgtype*)0)->msgnode)))

w_err_t wind_mbox_init(void);

mbox_s *wind_mbox_create(const char *name,thread_s *owner);
w_err_t wind_mbox_destroy(mbox_s *pmbox);

w_err_t wind_mbox_post(mbox_s *mbox,msg_s *pmsg);
w_err_t wind_mbox_fetch(mbox_s *mbox,msg_s **pmsg,w_uint32_t timeout);
/**/
#endif //WIND_MESSAGE_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_MESSAGE_H__

