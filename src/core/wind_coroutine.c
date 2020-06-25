/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_coroutine.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os coroutine function module
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
#include "wind_coroutine.h"
#include "wind_thread.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"
#include "wind_daemon.h"
#include "wind_board_port.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_COROUTINE_SUPPORT
#define NODE_TO_COROUTINE(node) NODEPTR_TO_ITEMPTR(node,w_coroutine_s,obj.objnode)
static w_dlist_s coroutinelist;
static w_coroutine_s *curcorout;
static WIND_POOL(coroutinepool,WIND_COROUTINE_MAX_NUM,sizeof(w_coroutine_s));
extern void wind_thread_switch(void);

static __INLINE__ w_coroutine_s *coroutine_malloc(void)
{
    return (w_coroutine_s*)wind_pool_malloc(coroutinepool);
}

static __INLINE__ w_err_t coroutine_free(void *coroutine)
{
    return wind_pool_free(coroutinepool,coroutine);
}

static w_coroutine_s *get_coroutint_ready(void)
{
    w_dnode_s *dnode;
    w_coroutine_s *coroutine;
    foreach_node(dnode,&coroutinelist)
    {
        coroutine = NODE_TO_COROUTINE(dnode);
        if(IS_F_COROUTINE_READY(coroutine))
        {
            return coroutine;
        }
    }
    return W_NULL;
}



w_err_t _wind_coroutine_mod_init(void)
{
    w_err_t err;
    curcorout = W_NULL;
    DLIST_INIT(coroutinelist);
    err = wind_pool_create("coroutine",coroutinepool,sizeof(coroutinepool),sizeof(w_coroutine_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}


w_err_t _wind_coroutine_dispatch(void)
{
    w_thread_s *thread;
    w_coroutine_s *coroutine;
    if(curcorout != W_NULL)
        gwind_cur_stack = &curcorout->stack;
    else
    {
        thread = wind_thread_current();
        WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
        gwind_cur_stack = &thread->stack_cur;
    }
    coroutine = get_coroutint_ready();
    if(coroutine == W_NULL)
        return W_ERR_FAIL;
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_FAIL);
    gwind_high_stack = &coroutine->stack;
    curcorout = coroutine;        
    wind_thread_switch();
    return W_ERR_OK;
}

w_coroutine_s *wind_coroutine_get(const char *name)
{
    w_thread_s *thread;
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != W_NULL,W_NULL);
	return (w_coroutine_s*)wind_obj_get(name,&thread->coroutlist);
}

w_coroutine_s *wind_coroutine_get_byid(w_cid_t cid)
{
    w_dnode_s *dnode;
    w_coroutine_s *coroutine;
    foreach_node(dnode,&coroutinelist)
    {
        coroutine = NODE_TO_COROUTINE(dnode);
        if(coroutine->cid == cid)
        {
            return coroutine;
        }
    }
    return W_NULL;
}

w_err_t wind_coroutine_init(w_coroutine_s *coroutine,const char *name,w_uint16_t cid,coroutine_fn func,void *arg)
{
    w_thread_s *thread;
    wind_notice("init coroutine:%s",name != W_NULL?name:"null");
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(func != W_NULL,W_ERR_NULL_PTR);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    coroutine->cid = cid;
    coroutine->func = func;
    coroutine->arg = arg;
    coroutine->stack = _wind_thread_stack_init((thread_run_f)func,arg,coroutine->stackbuff,WIND_COROUTINE_STKSIZE);
    wind_obj_init(&coroutine->obj,WIND_COROUTINE_MAGIC,name,&thread->coroutlist);
    SET_F_COROUTINE_READY(coroutine);
    CLR_F_OBJ_POOL(coroutine->obj);
    return W_ERR_OK;
}


w_coroutine_s *wind_coroutine_create(const char *name,w_uint16_t cid,coroutine_fn func,void *arg)
{
    w_err_t err;
    w_coroutine_s *coroutine;

    coroutine = coroutine_malloc();
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_NULL);
    wind_disable_switch();
    err = wind_coroutine_init(coroutine,name,cid,func,arg);
    if(err == W_ERR_OK)
    {
        SET_F_OBJ_POOL(coroutine->obj);
        wind_enable_switch();
        return coroutine;
    }
    coroutine_free(coroutine);
    wind_enable_switch();
    return W_NULL;
}


w_err_t wind_coroutine_destroy(w_coroutine_s *coroutine)
{
    w_err_t err;
    w_thread_s *thread;
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(coroutine->obj.magic == WIND_COROUTINE_MAGIC,W_ERR_INVALID);
    wind_notice("destroy coroutine:%s",wind_obj_name(coroutine));
    wind_disable_switch();
    err = wind_obj_deinit(&coroutine->obj,WIND_COROUTINE_MAGIC,&thread->coroutlist);
    //WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,wind_enable_switch(),W_ERR_FAIL);
    if(IS_F_OBJ_POOL(coroutine->obj))
        coroutine_free(coroutine);
    wind_enable_switch();
    return err;
}

w_err_t wind_coroutine_yeild(w_coroutine_s *coroutine,void *arg)
{
    w_thread_s *thread;
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(coroutine->obj.magic == WIND_COROUTINE_MAGIC,W_ERR_INVALID);
    CLR_F_COROUTINE_READY(coroutine);
    _wind_coroutine_dispatch();
    return W_ERR_OK;
}


w_err_t wind_coroutine_resume(w_coroutine_s *coroutine,void *arg)
{
    //w_dlist_s *sleeplist = _wind_thread_sleep_list();
    WIND_ASSERT_RETURN(coroutine != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(coroutine->obj.magic == WIND_COROUTINE_MAGIC,W_ERR_INVALID);
    SET_F_COROUTINE_READY(coroutine);
    return W_ERR_OK;
}


w_err_t wind_coroutine_print(void)
{
    w_dnode_s *dnode;
    w_coroutine_s *coroutine;
    w_dlist_s *list = &coroutinelist;
    wind_printf("\r\n\r\ncoroutine list:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-8s\r\n","coroutine","cid","status");
    wind_print_space(5);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        coroutine = NODE_TO_COROUTINE(dnode);
        wind_printf("%-16s %-8d %-8s\r\n",
            wind_obj_name(coroutine),coroutine->cid,IS_F_COROUTINE_READY(coroutine));
    }
    wind_enable_switch();
    wind_print_space(5);
    return W_ERR_OK;
}

#endif //if WIND_COROUTINE_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
