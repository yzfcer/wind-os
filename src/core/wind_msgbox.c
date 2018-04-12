/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_message.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的消息相关的代码
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

#include "wind_config.h"
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_msgbox.h"
#include "wind_core.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_debug.h"
#if WIND_MESSAGE_SUPPORT
extern void wind_thread_dispatch(void);
WIND_MPOOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(msgbox_s));

static msgbox_s *msgbox_malloc(void)
{
    return wind_pool_alloc(msgboxpool);
}

static w_err_t msgbox_free(msgbox_s *msgbox)
{
    return wind_pool_free(msgboxpool,msgbox);
}


//********************************************internal functions******************************

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t wind_msgbox_init(void)
{
    w_err_t err;
    err = wind_pool_create("msgbox",msgboxpool,sizeof(msgboxpool),sizeof(msgbox_s));
    return err;
}

void wind_msg_init(msg_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg)
{
    msg->msg_id = msg_id;
    msg->msg_len = msg_len;
    msg->msg_arg = msg_arg;
    DNODE_INIT(msg->msgnode);
}


//创建邮箱，只能在线程中创建，不能在中断中和线程运行之前
msgbox_s *wind_msgbox_create(const char *name,thread_s *owner)
{
    msgbox_s *pmsgbox;
    wind_notice("create msgbox:%s",name);
    pmsgbox = msgbox_malloc();
    WIND_ASSERT_RETURN(pmsgbox != NULL,NULL);

    pmsgbox->name = name;
    DNODE_INIT(pmsgbox->msgboxnode);
    DLIST_INIT(pmsgbox->msglist);
    pmsgbox->num = 0;
    pmsgbox->magic = WIND_MSGBOX_MAGIC;
    pmsgbox->owner = owner;
    wind_close_interrupt();
    dlist_insert_tail(&g_core.msgboxlist,&pmsgbox->msgboxnode);
    wind_open_interrupt();
    return pmsgbox;
}

w_err_t wind_msgbox_destroy(msgbox_s *pmsgbox)
{
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmsgbox != NULL,ERR_NULL_POINTER);
    wind_notice("destroy msgbox:%s",pmsgbox->name);
    pthread = pmsgbox->owner;
    if((pmsgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (pmsgbox->owner->cause == CAUSE_MSG))
    {
        pthread->runstat = THREAD_STATUS_READY;
    }

    wind_close_interrupt();
    dlist_remove_tail(&g_core.msgboxlist);
    pmsgbox->magic = 0;
    pmsgbox->owner = NULL;
    pmsgbox->name = NULL;
    dnode = dlist_head(&pmsgbox->msglist);
    if(dnode != NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",pmsgbox->name);
    }
    msgbox_free(pmsgbox);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_msgbox_post(msgbox_s *msgbox,msg_s *pmsg)
{
    thread_s *pthread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    //将消息加入邮箱
    wind_close_interrupt();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->num ++;

    //激活被阻塞的线程
    pthread = msgbox->owner;
    if((pthread->runstat != THREAD_STATUS_SLEEP) 
       || (pthread->cause != CAUSE_MSG))
    {
        wind_open_interrupt();
        return ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_open_interrupt();
    wind_thread_dispatch();//切换线程
    return ERR_OK;
}


w_err_t wind_msgbox_fetch(msgbox_s *msgbox,msg_s **pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    
    //如果邮箱中有消息，就直接返回消息
    wind_close_interrupt();
    if(msgbox->num > 0)
    {
        msgbox->num --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        return ERR_OK;
    }

    //否则将线程加入睡眠队列
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    pthread = msgbox->owner;
    pthread->runstat = THREAD_STATUS_SLEEP;
    pthread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&pthread->sleepthr.node);
    wind_open_interrupt();
    
    wind_thread_dispatch();
    if(pthread->cause == CAUSE_MSG)
    {
        if(msgbox->num <= 0)
        {
            pthread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        err = ERR_OK;
    }
    else if(pthread->cause == CAUSE_SLEEP)
    {
        err = ERR_TIMEOUT;
    }
    else
        err = ERR_COMMAN;
    
    return err;
}


#endif  //WIND_MESSAGE_SUPPORT

