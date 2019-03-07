/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_message.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的消息相关的代码头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_MESSAGE_H__
#define WIND_MESSAGE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_MSGBOX_SUPPORT
#define WIND_MSGBOX_MAGIC 0x378A523B

#define F_MSGBOX_POOL (0x01 << 0)
#define IS_F_MSGBOX_POOL(msgbox) ((msgbox->obj.flag & F_MSGBOX_POOL) == F_MSGBOX_POOL)
#define SET_F_MSGBOX_POOL(msgbox) (msgbox->obj.flag |= F_MSGBOX_POOL)
#define CLR_F_MSGBOX_POOL(msgbox) (msgbox->obj.flag &= (~F_MSGBOX_POOL))

typedef struct _w_msg_s
{
    w_dnode_s msgnode;
    w_uint16_t msg_id;
    w_uint16_t msg_len;
    void *msg_arg;
}w_msg_s; 

typedef struct __w_msgbox_s
{
    w_obj_s obj;
    w_dlist_s msglist;//消息队列
    w_int16_t msgnum;//消息的数量
    w_thread_s *owner;
}w_msgbox_s;



#define NODE_TO_MSGBOX(node) (w_msgbox_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_msgbox_s*)0)->obj.objnode)))
#define NODE_TO_MSG(dnode) (w_msg_s*)(((w_uint8_t*)(dnode))-((w_uint32_t)&(((w_msg_s*)0)->msgnode)))

void wind_msg_init(w_msg_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg);
w_err_t _wind_msgbox_mod_init(void);
w_msgbox_s *wind_msgbox_get(const char *name);
w_err_t wind_msgbox_init(w_msgbox_s *msgbox,const char *name);
w_msgbox_s *wind_msgbox_create(const char *name);
w_err_t wind_msgbox_trydestroy(w_msgbox_s *msgbox);
w_err_t wind_msgbox_destroy(w_msgbox_s *msgbox);

w_err_t wind_msgbox_post(w_msgbox_s *msgbox,w_msg_s *pmsg);
w_err_t wind_msgbox_wait(w_msgbox_s *msgbox,w_msg_s **pmsg,w_uint32_t timeout);
w_err_t wind_msgbox_trywait(w_msgbox_s *msgbox,w_msg_s **pmsg);
w_err_t wind_msgbox_print(void);

#endif //WIND_MSGBOX_SUPPORT

#ifdef __cplusplus
}
#endif
#endif  //WIND_MESSAGE_H__

