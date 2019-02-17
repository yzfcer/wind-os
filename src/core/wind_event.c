/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_event.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的事件对象相关功能
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
#include "wind_event.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_EVENT_SUPPORT
static w_dlist_s eventlist;
WIND_POOL(eventpool,WIND_EVENT_MAX_NUM,sizeof(w_event_s));

static w_event_s *event_malloc(void)
{
    return wind_pool_malloc(eventpool);
}

static w_err_t event_free(w_event_s *event)
{
    return wind_pool_free(eventpool,event);
}


w_err_t _wind_event_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(eventlist);
    err = wind_pool_create("event",eventpool,sizeof(eventpool),sizeof(w_event_s));
    return err;
}


w_event_s *wind_event_get(const char *name)
{
    w_event_s *event;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&eventlist)
    {
        event = DLIST_OBJ(dnode,w_event_s,eventnode);
        if(event->name && (wind_strcmp(name,event->name) == 0))
        {
            wind_enable_switch();
            return event;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_event_init(w_event_s *event,const char *name)
{
    wind_notice("init event:%s",name?name:"null");
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    event->magic = WIND_EVENT_MAGIC;
    event->name = name;
    DNODE_INIT(event->eventnode);
    DLIST_INIT(event->cblist);
    event->flag = 0;
    CLR_F_EV_POOL(event);
    wind_disable_interrupt();
    dlist_insert_tail(&eventlist,&event->eventnode);
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_event_s *wind_event_create(const char *name)
{
    w_err_t err;
    w_event_s *event;
    event = event_malloc();
    WIND_ASSERT_RETURN(event != W_NULL,W_NULL);
    err = wind_event_init(event,name);
    if(err == W_ERR_OK)
    {
        SET_F_EV_POOL(event);
        return event;
    }
    event_free(event);
    return event;
}



w_err_t wind_event_destroy(w_event_s *event)
{
    w_dnode_s *dnode;
    wind_notice("destroy event:%s",event->name?event->name:"null");
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    dlist_remove_tail(&eventlist);
    wind_enable_interrupt();

    event->magic = 0;
    dnode = dlist_head(&event->cblist);
    if(dnode != W_NULL)
    {
        wind_warn("event:%s is NOT empty while destroying it.",
            event->name?event->name:"null");
    }
    if(IS_F_EV_POOL(event))
        event_free(event);
    return W_ERR_OK;
}

w_err_t wind_event_regcb(w_event_s *event,w_event_cb *cb)
{
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    dlist_insert_tail(&event->cblist,&cb->listenernode);
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_err_t wind_event_unregcb(w_event_s *event,w_event_cb *cb)
{
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,W_ERR_INVALID);
    wind_disable_interrupt();
    dlist_remove(&event->cblist,&cb->listenernode);
    wind_enable_interrupt();
    return W_ERR_OK;
}

w_err_t wind_event_trig(w_event_s *event,void *arg)
{
    w_dnode_s *dnode;
    w_event_cb *cb;
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->magic == WIND_EVENT_MAGIC,W_ERR_FAIL);
    foreach_node(dnode,&event->cblist)
    {
        cb = DLIST_OBJ(dnode,w_event_cb,listenernode);
        if(cb != W_NULL)
            cb->cb_fn(event,arg);
    }
    return W_ERR_OK;
}


w_err_t wind_event_print(void)
{
    w_dnode_s *dnode;
    w_event_s *event;
    w_dlist_s *list = &eventlist;
    wind_printf("\r\n\r\nevent list as following:\r\n");
    wind_print_space(2);
    wind_printf("%-16s\r\n","event");
    wind_print_space(2);

    foreach_node(dnode,list)
    {
        event = (w_event_s *)DLIST_OBJ(dnode,w_event_s,eventnode);
        wind_printf("%-16s\r\n",event->name?event->name:"null");            
    }
    wind_print_space(2);
    return W_ERR_OK;
}

#endif  //WIND_EVENT_H__

