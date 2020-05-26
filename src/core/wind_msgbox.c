/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_message.c
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
#include "wind_msgbox.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_MSGBOX_SUPPORT
#define NODE_TO_MSGBOX(node) (w_msgbox_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_msgbox_s*)0)->obj.objnode)))
#define NODE_TO_MSG(dnode) (w_msg_s*)(((w_uint8_t*)(dnode))-((w_addr_t)&(((w_msg_s*)0)->msgnode)))
static w_dlist_s msgboxlist;
WIND_POOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(w_msgbox_s));

static w_msgbox_s *msgbox_malloc(void)
{
    return (w_msgbox_s *)wind_pool_malloc(msgboxpool);
}

static w_err_t msgbox_free(w_msgbox_s *msgbox)
{
    return wind_pool_free(msgboxpool,msgbox);
}



w_err_t _wind_msgbox_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(msgboxlist);
    err = wind_pool_create("msgbox",msgboxpool,sizeof(msgboxpool),sizeof(w_msgbox_s));
    return err;
}

void wind_msg_init(w_msg_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg)
{
    msg->msg_id = msg_id;
    msg->msg_len = msg_len;
    msg->msg_arg = msg_arg;
    DNODE_INIT(msg->msgnode);
}

w_msgbox_s *wind_msgbox_get(const char *name)
{
    return (w_msgbox_s*)wind_obj_get(name,&msgboxlist);
}

w_err_t wind_msgbox_init(w_msgbox_s *msgbox,const char *name)
{
    wind_notice("init msgbox:%s",name?name:"null");
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    DLIST_INIT(msgbox->msglist);
    msgbox->msgnum = 0;
    CLR_F_MSGBOX_POOL(msgbox);
    msgbox->owner = wind_thread_current();
    wind_obj_init(&msgbox->obj,WIND_MSGBOX_MAGIC,name,&msgboxlist);

    return W_ERR_OK;
}


w_msgbox_s *wind_msgbox_create(const char *name)
{
    w_err_t err;
    w_msgbox_s *msgbox;
    
    msgbox = msgbox_malloc();
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_NULL);
    err = wind_msgbox_init(msgbox,name);
    if(err == W_ERR_OK)
    {
        SET_F_MSGBOX_POOL(msgbox);
        return msgbox;
    }
    msgbox_free(msgbox);
    return W_NULL;
}


w_err_t wind_msgbox_trydestroy(w_msgbox_s *msgbox)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->obj.magic == WIND_MSGBOX_MAGIC,W_ERR_INVALID);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    wind_disable_switch();
    dnode = dlist_head(&msgbox->msglist);
    if(dnode != W_NULL)
    {
        wind_enable_switch();
        return W_ERR_FAIL;
    }
    wind_enable_switch();
    return wind_msgbox_destroy(msgbox);
}

w_err_t wind_msgbox_destroy(w_msgbox_s *msgbox)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->obj.magic == WIND_MSGBOX_MAGIC,W_ERR_INVALID);
    wind_notice("destroy msgbox:%s",wind_obj_name(&msgbox->obj));
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    err = wind_obj_deinit(&msgbox->obj,WIND_MSGBOX_MAGIC,&msgboxlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    
    thread = msgbox->owner;
    if((msgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (msgbox->owner->cause == CAUSE_MSG))
    {
        thread->runstat = THREAD_STATUS_READY;
    }

    dnode = dlist_head(&msgbox->msglist);
    if(dnode != W_NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",wind_obj_name(&msgbox->obj));
    }
    if(IS_F_MSGBOX_POOL(msgbox))
        msgbox_free(msgbox);
    return W_ERR_OK;
}

w_err_t wind_msgbox_post(w_msgbox_s *msgbox,w_msg_s *pmsg)
{
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pmsg != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->obj.magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    wind_disable_switch();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->msgnum ++;

    thread = msgbox->owner;
    if((thread->runstat != THREAD_STATUS_SLEEP) 
       || (thread->cause != CAUSE_MSG))
    {
        wind_enable_switch();
        return W_ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_enable_switch();
    _wind_thread_dispatch();
    return W_ERR_OK;
}


w_err_t wind_msgbox_wait(w_msgbox_s *msgbox,w_msg_s **pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    w_dnode_s *dnode;
    w_thread_s *thread;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pmsg != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->obj.magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    wind_disable_switch();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = NODE_TO_MSG(dnode);
        wind_enable_switch();
        return W_ERR_OK;
    }

    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    thread = msgbox->owner;
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_MSG;
    dlist_insert_tail(sleeplist,&thread->sleepnode.dnode);
    wind_enable_switch();
    
    _wind_thread_dispatch();
    if(thread->cause == CAUSE_MSG)
    {
        if(msgbox->msgnum <= 0)
        {
            thread->runstat = THREAD_STATUS_READY;
            return W_ERR_PTR_NULL;
        }
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = NODE_TO_MSG(dnode);
        wind_enable_switch();
        err = W_ERR_OK;
    }
    else if(thread->cause == CAUSE_SLEEP)
    {
        err = W_ERR_TIMEOUT;
    }
    else
        err = W_ERR_FAIL;
    
    return err;
}

w_err_t wind_msgbox_trywait(w_msgbox_s *msgbox,w_msg_s **pmsg)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(pmsg != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->obj.magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    wind_disable_switch();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = NODE_TO_MSG(dnode);
        err = W_ERR_OK;
    }
    else
    {
        *pmsg = W_NULL;
        err = W_ERR_FAIL;
    }
    wind_enable_switch();
    return err;
}

w_err_t wind_msgbox_print_list(void)
{
    return wind_obj_print_list(&msgboxlist);
}


w_err_t wind_msgbox_print_detail(void)
{
    w_dnode_s *dnode;
    w_msgbox_s *msgbox;
    w_dlist_s *list = &msgboxlist;
    wind_printf("\r\n\r\nmsgbox list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s\r\n","msgbox","msg_num","owner");
    wind_print_space(5);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        msgbox = NODE_TO_MSGBOX(dnode);
        wind_printf("%-16s %-8d %-16s\r\n",
            wind_obj_name(&msgbox->obj),msgbox->msgnum,
            msgbox->owner->name?msgbox->owner->name:"null");
    }
    wind_enable_switch();
    wind_print_space(5);
    return W_ERR_OK;
}

#endif  //WIND_MSGBOX_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
