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
#include "wind_types.h"
#include "wind_list.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_MESSAGE_SUPPORT > 0

typedef struct _wind_message
{
    const char *name;
    bool_t used;
    u16_t validtime;
    u16_t msgNO;
    u16_t param;
    void *lparam;
}msg_s, *pmsg_s; 

typedef struct __mbox_s
{
    const char *name;
    int num;//消息的数量
    bool_t used;
    bool_t valid;//邮箱是否可用
    pnode_s msgq;//消息队列
    pthread_s owner;
}mbox_s, *pmbox_s;


pmsg_s wind_message_create(const char *name,u16_t msgNo,u16_t param,void *lparam);
err_t wind_message_destroy(pmsg_s pmsg);

pmbox_s wind_mbox_create(const char *name);
err_t wind_mbox_destroy(pmbox_s pmbox);

err_t wind_mbox_post(pmbox_s mbox,pmsg_s pmsg);
err_t wind_mbox_fetch(pmbox_s mbox,pmsg_s *pmsg,u32_t timeout);
/**/
#endif //WIND_MESSAGE_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_MESSAGE_H__

