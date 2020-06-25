/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_message.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os message mailbox function set
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
#ifndef WIND_MESSAGE_H__
#define WIND_MESSAGE_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_thread.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_MSGBOX_SUPPORT
#define WIND_MSGBOX_MAGIC 0x378A523B

#define F_MSGBOX_POOL (0x01 << 0) //Mark whether the msgbox object is allocated through the memory pool
#define IS_F_MSGBOX_POOL(msgbox) ((msgbox->obj.userflag & F_MSGBOX_POOL) == F_MSGBOX_POOL)
#define SET_F_MSGBOX_POOL(msgbox) (msgbox->obj.userflag |= F_MSGBOX_POOL)
#define CLR_F_MSGBOX_POOL(msgbox) (msgbox->obj.userflag &= (~F_MSGBOX_POOL))

//Message header information
typedef struct _w_msg_s
{
    w_dnode_s msgnode;  //Message list node
    w_uint16_t msg_id;  //Message ID
    w_uint16_t msg_len; //Message lenth
    void *msg_arg;      //Message cotent
}w_msg_s; 

//Mailbox object results
typedef struct __w_msgbox_s
{
    w_obj_s obj;       //Basic object information
    w_dlist_s msglist; //Message queue list
    w_int16_t msgnum;  //Number of messages
    w_thread_s *owner; //Owner of mailbox
}w_msgbox_s;


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
w_err_t wind_msgbox_print_list(void);
w_err_t wind_msgbox_print_detail(void);

#endif // #if WIND_MSGBOX_SUPPORT

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif  // #ifndef WIND_MESSAGE_H__

