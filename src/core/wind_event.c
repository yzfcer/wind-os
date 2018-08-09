/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_event.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的事件对象相关功能
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
#include "wind_event.h"
#include "wind_core.h"
#include "wind_var.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_EVENT_SUPPORT
extern void _wind_thread_dispatch(void);
WIND_POOL(eventpool,WIND_EVENT_MAX_NUM,sizeof(event_s));

static event_s *event_malloc(void)
{
    return wind_pool_malloc(eventpool);
}

static w_err_t event_free(event_s *event)
{
    return wind_pool_free(eventpool,event);
}


//********************************************internal functions******************************

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t _wind_event_init(void)
{
    w_err_t err;
    err = wind_pool_create("event",eventpool,sizeof(eventpool),sizeof(event_s));
    return err;
}


event_s *wind_event_get(const char *name)
{
    event_s *event;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.eventlist)
    {
        event = DLIST_OBJ(dnode,event_s,eventnode);
        if(wind_strcmp(name,event->name) == 0)
        {
            wind_enable_switch();
            return event;
        }
    }
    wind_enable_switch();
    return NULL;
}

//创建邮箱，创建邮箱的那个线程才能从中读取消息
event_s *wind_event_create(const char *name)
{
    event_s *event;
    wind_notice("create event:%s",name);
    event = event_malloc();
    WIND_ASSERT_RETURN(event != NULL,NULL);

    event->name = name;
    DNODE_INIT(event->eventnode);
    DLIST_INIT(event->msglist);
    event->msgnum = 0;
    event->magic = WIND_EVENT_MAGIC;
    event->owner = wind_thread_current();
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.eventlist,&event->eventnode);
    wind_enable_interrupt();
    return event;
}



w_err_t wind_event_destroy(event_s *event)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(event != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,ERR_INVALID_PARAM);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(event->owner == thread,ERR_FAIL);
    wind_notice("destroy event:%s",event->name);
    wind_disable_interrupt();
    dlist_remove_tail(&g_core.eventlist);
    wind_enable_interrupt();
    thread = event->owner;
    if((event->owner->runstat == THREAD_STATUS_SLEEP) 
       && (event->owner->cause == CAUSE_MSG))
    {
        thread->runstat = THREAD_STATUS_READY;
    }

    event->magic = 0;
    dnode = dlist_head(&event->msglist);
    if(dnode != NULL)
    {
        wind_warn("event:%s is NOT empty while destroying it.",event->name);
    }
    event_free(event);
    return ERR_OK;
}

w_err_t wind_event_trig(event_s *event,const void *arg)
{
    thread_s *thread;
    WIND_ASSERT_RETURN(event != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,ERR_FAIL);
    WIND_ASSERT_RETURN(event->owner,ERR_FAIL);
    //将消息加入邮箱
    wind_disable_interrupt();
    dlist_insert_tail(&event->msglist,&pmsg->msgnode);
    event->msgnum ++;

    //激活被阻塞的线程
    thread = event->owner;
    if((thread->runstat != THREAD_STATUS_SLEEP) 
       || (thread->cause != CAUSE_MSG))
    {
        wind_enable_interrupt();
        return ERR_OK;
    }
    event->owner->runstat = THREAD_STATUS_READY;
    wind_enable_interrupt();
    _wind_thread_dispatch();//切换线程
    return ERR_OK;
}


w_err_t wind_event_wait(event_s *event,void **arg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(event != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,ERR_FAIL);
    WIND_ASSERT_RETURN(event->owner,ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(event->owner == thread,ERR_FAIL);
    //如果邮箱中有消息，就直接返回消息
    wind_disable_interrupt();
    if(event->msgnum > 0)
    {
        event->msgnum --;
        dnode = dlist_remove_head(&event->msglist);
        *pmsg = DLIST_OBJ(dnode,listener_s,msgnode);
        wind_enable_interrupt();
        return ERR_OK;
    }

    //否则将线程加入睡眠队列
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    thread = event->owner;
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&thread->sleepnode.dnode);
    wind_enable_interrupt();
    
    _wind_thread_dispatch();
    if(thread->cause == CAUSE_MSG)
    {
        if(event->msgnum <= 0)
        {
            thread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        dnode = dlist_remove_head(&event->msglist);
        *pmsg = DLIST_OBJ(dnode,listener_s,msgnode);
        wind_enable_interrupt();
        err = ERR_OK;
    }
    else if(thread->cause == CAUSE_SLEEP)
    {
        err = ERR_TIMEOUT;
    }
    else
        err = ERR_FAIL;
    
    return err;
}


w_err_t wind_event_print(dlist_s *list)
{
    dnode_s *dnode;
    event_s *event;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nevent list as following:\r\n");
    wind_print_space(6);
    wind_printf("%-16s %-8s %-16s \r\n","event","msg_num","owner");
    wind_print_space(6);

    foreach_node(dnode,list)
    {
        event = (event_s *)DLIST_OBJ(dnode,event_s,eventnode);
        wind_printf("%-16s %-8d %-16s\r\n",
            event->name,event->msgnum,event->owner->name);
    }
    wind_print_space(6);
    return ERR_OK;
}

#endif  //WIND_MSGBOX_SUPPORT

