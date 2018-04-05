/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_message.h
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
    dlist_s msglist;//消息队列
    int num;//消息的数量
    w_bool_t valid;//邮箱是否可用
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

