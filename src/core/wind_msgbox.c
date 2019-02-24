/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_message.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的消息相关的代码
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
#include "wind_msgbox.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_MSGBOX_SUPPORT
static w_dlist_s msgboxlist;
WIND_POOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(w_msgbox_s));

static w_msgbox_s *msgbox_malloc(void)
{
    return wind_pool_malloc(msgboxpool);
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
    w_msgbox_s *msgbox;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&msgboxlist)
    {
        msgbox = DLIST_OBJ(dnode,w_msgbox_s,msgboxnode);
        if(msgbox->name && (wind_strcmp(name,msgbox->name) == 0))
        {
            wind_enable_switch();
            return msgbox;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_msgbox_init(w_msgbox_s *msgbox,const char *name)
{
    wind_notice("init msgbox:%s",name?name:"null");
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    msgbox->magic = WIND_MSGBOX_MAGIC;
    msgbox->name = name;
    DNODE_INIT(msgbox->msgboxnode);
    DLIST_INIT(msgbox->msglist);
    msgbox->msgnum = 0;
    msgbox->flag = 0;
    CLR_F_MSGBOX_POOL(msgbox);
    msgbox->owner = wind_thread_current();
    wind_disable_interrupt();
    dlist_insert_tail(&msgboxlist,&msgbox->msgboxnode);
    wind_enable_interrupt();
    return W_ERR_OK;
}


//创建邮箱，创建邮箱的那个线程才能从中读取消息
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
    w_dnode_s *pdnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,W_ERR_INVALID);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    wind_disable_interrupt();
    pdnode = dlist_head(&msgbox->msglist);
    if(pdnode != W_NULL)
    {
        wind_enable_interrupt();
        return W_ERR_FAIL;
    }
    wind_enable_interrupt();
    return wind_msgbox_destroy(msgbox);
}

w_err_t wind_msgbox_destroy(w_msgbox_s *msgbox)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,W_ERR_INVALID);
    wind_notice("destroy msgbox:%s",msgbox->name?msgbox->name:"null");
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    
    wind_disable_interrupt();
    dlist_remove(&msgboxlist,&msgbox->msgboxnode);
    wind_enable_interrupt();
    msgbox->magic = (~WIND_MSGBOX_MAGIC);
    thread = msgbox->owner;
    if((msgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (msgbox->owner->cause == CAUSE_MSG))
    {
        thread->runstat = THREAD_STATUS_READY;
    }

    dnode = dlist_head(&msgbox->msglist);
    if(dnode != W_NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",msgbox->name?msgbox->name:"null");
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
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    //将消息加入邮箱
    wind_disable_interrupt();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->msgnum ++;

    //激活被阻塞的线程
    thread = msgbox->owner;
    if((thread->runstat != THREAD_STATUS_SLEEP) 
       || (thread->cause != CAUSE_MSG))
    {
        wind_enable_interrupt();
        return W_ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_enable_interrupt();
    _wind_thread_dispatch();//切换线程
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
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    //如果邮箱中有消息，就直接返回消息
    wind_disable_interrupt();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,w_msg_s,msgnode);
        wind_enable_interrupt();
        return W_ERR_OK;
    }

    //否则将线程加入睡眠队列
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    thread = msgbox->owner;
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_MSG;
    dlist_insert_tail(sleeplist,&thread->sleepnode.dnode);
    wind_enable_interrupt();
    
    _wind_thread_dispatch();
    if(thread->cause == CAUSE_MSG)
    {
        if(msgbox->msgnum <= 0)
        {
            thread->runstat = THREAD_STATUS_READY;
            return W_ERR_PTR_NULL;
        }
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,w_msg_s,msgnode);
        wind_enable_interrupt();
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
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,W_ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,W_ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,W_ERR_FAIL);
    //如果邮箱中有消息，就直接返回消息
    wind_disable_interrupt();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,w_msg_s,msgnode);
        err = W_ERR_OK;
    }
    else
    {
        *pmsg = W_NULL;
        err = W_ERR_FAIL;
    }
    wind_enable_interrupt();
    return err;
}

w_err_t wind_msgbox_print(void)
{
    w_dnode_s *dnode;
    w_msgbox_s *msgbox;
    w_dlist_s *list = &msgboxlist;
    wind_printf("\r\n\r\nmsgbox list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s\r\n","msgbox","msg_num","owner");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        msgbox = (w_msgbox_s *)DLIST_OBJ(dnode,w_msgbox_s,msgboxnode);
        wind_printf("%-16s %-8d %-16s\r\n",
            msgbox->name?msgbox->name:"null",msgbox->msgnum,
            msgbox->owner->name?msgbox->owner->name:"null");
    }
    wind_print_space(5);
    return W_ERR_OK;
}

#endif  //WIND_MSGBOX_SUPPORT

