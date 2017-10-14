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
#include "wind_message.h"
#include "wind_core.h"
#include "wind_list.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_err.h"
#include "wind_assert.h"
#if WIND_MESSAGE_SUPPORT > 0
extern void wind_thread_dispatch(void);

static pmsg_s msg_malloc(void)
{
    return (pmsg_s)wind_core_alloc(STAT_MESSAGE);
}

static w_err_t msg_free(pmsg_s msg)
{
    return wind_core_free(STAT_MESSAGE,(void *)msg);
}

static pmbox_s mbox_malloc(void)
{
    return (pmbox_s)wind_core_alloc(STAT_MBOX);
}

static w_err_t mbox_free(pmbox_s mbox)
{
    return wind_core_free(STAT_MBOX,(void *)mbox);
}

//********************************************internal functions******************************


static w_err_t mbox_delete_msgs(pnode_s list)
{
    pnode_s pnode = list,pnode1;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    pnode1 = pnode->next;
    while(pnode)
    {
        wind_core_free(STAT_MESSAGE,(void *)(pmsg_s)(pnode->obj));
        msg_free((void *)(pmsg_s)(pnode->obj));
        wind_node_free(pnode);
        pnode = pnode1;
        pnode1 = pnode->next;
    }
    return ERR_OK;
}
//********************************************internal functions******************************


//**********************************************extern functions******************************

//创建一个消息，并返回消息
pmsg_s wind_message_create(const char *name,w_uint16_t msgNo,w_uint16_t param,void *lparam)
{
    pmsg_s pmsg;
    pmsg = wind_core_alloc(STAT_MESSAGE);
    WIND_ASSERT_RETURN(pmsg != NULL,NULL);
    pmsg->name = name;
    pmsg->used = B_TRUE;
    //设定当前的线程句柄
    pmsg->validtime = 3000;
    pmsg->msgNO = msgNo;
    pmsg->param = param;
    pmsg->lparam = lparam;
    pmsg->used = B_TRUE;
    return pmsg;
}

w_err_t wind_message_destroy(pmsg_s pmsg)
{
    w_err_t err;
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    pmsg->used = B_FALSE;
    pmsg->name = NULL;
    err = wind_core_free(STAT_MESSAGE,pmsg);
    return err;
}


//创建邮箱，只能在线程中创建，不能再中断中和线程运行之前
pmbox_s wind_mbox_create(const char *name)
{
    pmbox_s pmbox;
    pmbox = wind_core_alloc(STAT_MBOX);
    WIND_ASSERT_RETURN(pmbox != NULL,NULL);

    pmbox->name = name;
    pmbox->used = B_TRUE;
    pmbox->owner = wind_thread_current();
    pmbox->num = 0;
    wind_list_init(&pmbox->msgq);
    pmbox->valid = B_TRUE;
    return pmbox;
}

w_err_t wind_mbox_destroy(pmbox_s pmbox)
{
    w_err_t err;
    pthread_s pthread;
    WIND_ASSERT_RETURN(pmbox != NULL,ERR_NULL_POINTER);
    pthread = wind_thread_current();
    WIND_ASSERT_RETURN(pthread == pmbox->owner,ERR_COMMAN);
    wind_close_interrupt();
    pmbox->valid = B_FALSE;
    pmbox->owner = NULL;
    pmbox->name = NULL;
    err = mbox_delete_msgs(list_head(&pmbox->msgq));
    WIND_ASSERT_RETURN(err == ERR_OK,err);
    wind_core_free(STAT_MBOX,pmbox);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_mbox_post(pmbox_s mbox,pmsg_s pmsg)
{
    pnode_s pnode;
    pthread_s pthread;
    WIND_ASSERT_RETURN(mbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(mbox->valid,ERR_COMMAN);
    WIND_ASSERT_RETURN(mbox->owner,ERR_COMMAN);
    
    pmsg->validtime = WIND_TICK_PER_SEC * 3;
    pnode = wind_node_malloc(CORE_TYPE_MSG);
    WIND_ASSERT_RETURN(pnode != NULL,ERR_NULL_POINTER);
    wind_node_bindobj(pnode,CORE_TYPE_MSG,0,pmsg);
    wind_list_inserttoend(&mbox->msgq,pnode);
    pthread = mbox->owner;
    pthread->cause = CAUSE_MSG;
    pthread->runstat = THREAD_STATUS_READY;
    wind_thread_dispatch();//切换线程
    return ERR_OK;
}


w_err_t wind_mbox_fetch(pmbox_s mbox,pmsg_s *pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    pnode_s pnode;
    pthread_s pthread;
    WIND_ASSERT_RETURN(mbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(mbox->valid,ERR_COMMAN);
    WIND_ASSERT_RETURN(mbox->owner,ERR_COMMAN);
    pthread = wind_thread_current();
    WIND_ASSERT_RETURN(mbox->owner == pthread,ERR_COMMAN);
    
    //如果邮箱中有消息，就直接返回消息
    
    if(list_head(&mbox->msgq) != NULL)
    {
        pnode = wind_list_remove(&mbox->msgq,list_head(&mbox->msgq));
        *pmsg = (pmsg_s)pnode->obj;
        wind_node_free(pnode);
        return ERR_OK;
    }
    //否则将线程加入睡眠队列
    ticks = timeout * WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    pthread = mbox->owner;
    pthread->runstat = THREAD_STATUS_SUSPEND;
    pthread->cause = CAUSE_MSG;
    
    wind_thread_dispatch();
    if(pthread->cause == CAUSE_MSG)
    {
        //几乎不会发生邮箱为空的情况
        if(list_head(&mbox->msgq) == NULL)
        {
            pthread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        pnode = wind_list_remove(&mbox->msgq,list_head(&mbox->msgq));
        *pmsg = (pmsg_s)pnode->obj;
        wind_node_free(pnode);
        err = ERR_OK;
    }
    else if(pthread->cause == CAUSE_SLEEP)
    {
        err = ERR_OK;
    }
    
    return err;
}


#endif  //WIND_MESSAGE_SUPPORT

