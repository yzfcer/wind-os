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
#define NODE_TO_EVET(node) (w_event_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_event_s*)0)->obj.objnode)))
#define NODE_TO_EVCB(node) (w_event_cb*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_event_cb*)0)->listenernode)))
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
    return (w_event_s*)wind_obj_get(name,&eventlist);
}

w_err_t wind_event_init(w_event_s *event,const char *name)
{
    wind_notice("init event:%s",name?name:"null");
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    DLIST_INIT(event->cblist);
    event->cbcnt = 0;
    wind_obj_init(&event->obj, WIND_EVENT_MAGIC,name,&eventlist);
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
        SET_F_EVENT_POOL(event);
        return event;
    }
    event_free(event);
    return event;
}



w_err_t wind_event_destroy(w_event_s *event)
{
    w_err_t err;
    w_dnode_s *dnode;
    wind_notice("destroy event:%s",event->obj.name?event->obj.name:"null");
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    err = wind_obj_deinit(&event->obj, WIND_EVENT_MAGIC,&eventlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    
    dnode = dlist_head(&event->cblist);
    if(dnode != W_NULL)
    {
        wind_warn("event:%s is NOT empty while destroying it.",
            event->obj.name?event->obj.name:"null");
    }
    if(IS_F_EVENT_POOL(event))
        event_free(event);
    event->cbcnt = 0;
    return W_ERR_OK;
}

w_err_t wind_event_regcb(w_event_s *event,w_event_cb *cb)
{
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->obj.magic == WIND_EVENT_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    dlist_insert_tail(&event->cblist,&cb->listenernode);
    event->cbcnt ++;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_event_unregcb(w_event_s *event,w_event_cb *cb)
{
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cb != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->obj.magic == WIND_EVENT_MAGIC,W_ERR_INVALID);
    wind_disable_switch();
    dlist_remove(&event->cblist,&cb->listenernode);
    event->cbcnt --;
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_event_trig(w_event_s *event,void *arg)
{
    w_dnode_s *dnode;
    w_event_cb *cb;
    WIND_ASSERT_RETURN(event != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(event->obj.magic == WIND_EVENT_MAGIC,W_ERR_FAIL);
    foreach_node(dnode,&event->cblist)
    {
        cb = NODE_TO_EVCB(dnode);
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
    wind_printf("\r\n\r\nevent list:\r\n");
    wind_print_space(2);
    wind_printf("%-16s\r\n","event");
    wind_print_space(2);

    foreach_node(dnode,list)
    {
        event = NODE_TO_EVET(dnode);
        wind_printf("%-16s\r\n",event->obj.name?event->obj.name:"null");            
    }
    wind_print_space(2);
    return W_ERR_OK;
}

#endif  //WIND_EVENT_H__

