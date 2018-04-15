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

#if WIND_MSGBOX_SUPPORT
#define WIND_MSGBOX_MAGIC 0x378A523B
typedef struct _wind_message
{
    dnode_s msgnode;
    w_uint16_t msg_id;
    w_uint16_t msg_len;
    void *msg_arg;
}msg_s; 

typedef struct __msgbox_s
{
    w_uint32_t magic;//ħ����
    const char *name;
    dnode_s msgboxnode;
    dlist_s msglist;//��Ϣ����
    int num;//��Ϣ������
    thread_s *owner;
}msgbox_s;


#define GET_MSG(nodeptr,msgtype,msgnode) (void*)(((char*)(nodeptr))-((w_uint32_t)&(((msgtype*)0)->msgnode)))

w_err_t _wind_msgbox_init(void);

void wind_msg_init(msg_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg);

msgbox_s *wind_msgbox_create(const char *name,thread_s *owner);
w_err_t wind_msgbox_destroy(msgbox_s *msgbox);

w_err_t wind_msgbox_post(msgbox_s *msgbox,msg_s *pmsg);
w_err_t wind_msgbox_wait(msgbox_s *msgbox,msg_s **pmsg,w_uint32_t timeout);
w_err_t wind_msgbox_trywait(msgbox_s *msgbox,msg_s **pmsg);
w_err_t _wind_msgbox_print(dlist_s *list);

#else
#define _wind_pipe_init() ERR_OK
#endif //WIND_MSGBOX_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_MESSAGE_H__

