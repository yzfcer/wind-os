/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_coroutine.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的协程功能模块
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
#include "wind_coroutine.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_COROUTINE_SUPPORT
#define NODE_TO_COROUTINE(node) (w_coroutine_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_coroutine_s*)0)->obj.objnode)))
static w_dlist_s coroutinelist;
static WIND_POOL(coroutinepool,WIND_COROUTINE_MAX_NUM,sizeof(w_coroutine_s));

static __INLINE__ w_coroutine_s *coroutine_malloc(void)
{
    return (w_coroutine_s*)wind_pool_malloc(coroutinepool);
}

static __INLINE__ w_err_t coroutine_free(void *coroutine)
{
    return wind_pool_free(coroutinepool,coroutine);
}

void _wind_coroutine_dispatch(void)
{
#if 0//WIND_REALTIME_CORE_SUPPORT
    w_thread_s *thread;
    if(RUN_FLAG == W_FALSE)
        return;
    wind_disable_interrupt();
    if(!is_switch_enable())
    {
        wind_enable_interrupt();
        return;
    }
    thread = wind_search_highthread();
    if(thread == wind_thread_current())
    {
        wind_enable_interrupt();
        return;
    }
    gwind_high_stack = &thread->stack_cur;
    if(gwind_high_stack != gwind_cur_stack)
    {
        wind_enable_interrupt();
        wind_thread_switch();
    }
    else
        wind_enable_interrupt();
#endif
}


w_err_t _wind_coroutine_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(coroutinelist);
    err = wind_pool_create("coroutine",coroutinepool,sizeof(coroutinepool),sizeof(w_coroutine_s));
    return err;
}

w_coroutine_s *wind_coroutine_get(const char *name)
{
	return (w_coroutine_s*)wind_obj_get(name,&coroutinelist);
}

w_coroutine_s *wind_coroutine_get_byid(w_cid_t cid)
{
    return W_NULL;
	//return (w_coroutine_s*)wind_obj_get(name,&coroutinelist);
}

w_err_t wind_coroutine_init(w_coroutine_s *coroutine,const char *name,w_uint16_t cid,coroutine_fn func,void *arg)
{
    w_thread_s *thread;
    wind_notice("init coroutine:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_PTR_NULL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    coroutine->cid = cid;
    coroutine->thread = thread;
    coroutine->func = func;
    coroutine->arg = arg;
    coroutine->stack = _wind_thread_stack_init(func,arg,&coroutine->stackbuff[WIND_COROUTINE_STKSIZE-1]);
    SET_F_COROUTINE_READY(coroutine);
    wind_obj_init(&coroutine->obj,WIND_COROUTINE_MAGIC,name,&coroutinelist);
    CLR_F_COROUTINE_POOL(coroutine);
    return W_ERR_OK;
}

w_coroutine_s *wind_coroutine_create(const char *name,w_uint16_t cid,coroutine_fn func,void *arg)
{
    w_err_t err;
    w_coroutine_s *coroutine;

    coroutine = coroutine_malloc();
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_NULL);
    err = wind_coroutine_init(coroutine,name,cid,func,arg);
    if(err == W_ERR_OK)
    {
        SET_F_COROUTINE_POOL(coroutine);
        return coroutine;
    }
    coroutine_free(coroutine);
    return W_NULL;
}


w_err_t wind_coroutine_destroy(w_coroutine_s *coroutine)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_PTR_NULL);
    wind_notice("destroy coroutine:%s",coroutine->obj.name != W_NULL?coroutine->obj.name:"null");
    err = wind_obj_deinit(&coroutine->obj,WIND_COROUTINE_MAGIC,&coroutinelist);
    WIND_ASSERT_RETURN(err == W_ERR_OK, W_ERR_FAIL);
    if(IS_F_COROUTINE_POOL(coroutine))
        coroutine_free(coroutine);
    return W_ERR_OK;
}

w_err_t wind_coroutine_yeild(w_coroutine_s *coroutine,void *arg)
{
    w_dnode_s *dnode;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(coroutine->obj.magic == WIND_COROUTINE_MAGIC,W_ERR_INVALID);
    _wind_coroutine_dispatch();
    return W_ERR_OK;
}


w_err_t wind_coroutine_resume(w_coroutine_s *coroutine,void *arg)
{
    w_thread_s *thread;
    w_dlist_s *sleeplist = _wind_thread_sleep_list();
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(coroutine->obj.magic == WIND_COROUTINE_MAGIC,W_ERR_INVALID);
    return W_ERR_OK;
}


w_err_t wind_coroutine_print(void)
{
    w_dnode_s *dnode;
    w_coroutine_s *coroutine;
    w_dlist_s *list = &coroutinelist;
    wind_printf("\r\n\r\ncoroutine list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s\r\n","coroutine","cid","thread");
    wind_print_space(5);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        coroutine = NODE_TO_COROUTINE(dnode);
        wind_printf("%-16s %-8d %-16s\r\n",
            wind_obj_name(coroutine),coroutine->cid,wind_obj_name(coroutine->thread));
    }
    wind_enable_switch();
    wind_print_space(5);
    return W_ERR_OK;
}

#endif

