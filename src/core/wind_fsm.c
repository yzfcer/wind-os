/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: wind_fsm.c
** 创   建   人: 周江村
** 最后修改日期: 2019/12/1 15:34:56
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2019/12/1 15:34:56
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 有限状态机功能模型框架
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_fsm.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_obj.h"
#include "wind_pool.h"
#if WIND_FSM_SUPPORT

#define NODE_TO_FSM(node) (w_fsm_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_fsm_s*)0)->obj.objnode)))


/********************************************内部变量定义**********************************************/
static w_dlist_s modellist;
static w_dlist_s fsmlist;
static w_fsm_s *cur_fsm;
static WIND_POOL(fsmpool,WIND_FSM_MAX_NUM,sizeof(w_fsm_s));



/********************************************内部函数定义*********************************************/
static w_fsm_s *fsm_malloc()
{
    return (w_fsm_s *)wind_pool_malloc(fsmpool);
}

static w_err_t fsm_free(w_fsm_s *fsm)
{
    return wind_pool_free(fsmpool,fsm);
}

static w_fsm_s *get_reay_fsm(void)
{
    w_fsm_s *fsm;
    w_dnode_s *dnode = &cur_fsm->obj.objnode;
    for(;dnode != W_NULL;dnode = dnode->next)
    {
        fsm = NODE_TO_FSM(dnode);
        if(fsm->state == FSM_STAT_READY)
            return fsm;
    }
    for(dnode = fsmlist.head;dnode != &cur_fsm->obj.objnode;dnode = dnode->next)
    {
        fsm = NODE_TO_FSM(dnode);
        if(fsm->state == FSM_STAT_READY)
            return fsm;
    }
    return (w_fsm_s *)W_NULL;
}

static char *fsm_state_str(w_fsm_state_e state)
{
    if(state == FSM_STAT_IDLE)
        return "idle";
    else if(state == FSM_STAT_READY)
        return "ready";
    else if(state == FSM_STAT_SLEEP)
        return "sleep";
    else if(state == FSM_STAT_SUSPEND)
        return "suspend";
    else if(state == FSM_STAT_STOP)
        return "stop";
    return "unknown";
}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
w_err_t _wind_fsm_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(modellist);
    DLIST_INIT(fsmlist);
    cur_fsm = (w_fsm_s*)W_NULL;
    err = wind_pool_create("fsm",fsmpool,sizeof(fsmpool),sizeof(w_fsm_s));
    return err;
}

w_err_t wind_fsm_model_register(w_fsm_model_s *fsm_model)
{
    w_fsm_model_s *model;    
    WIND_ASSERT_RETURN(fsm_model != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm_model->obj.magic == (~WIND_FSM_MODEL_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(fsm_model->obj.name != W_NULL,W_ERR_PTR_NULL);
    wind_notice("register fsm_model:%s",wind_obj_name(&fsm_model->obj));
    model = wind_fsm_model_get(fsm_model->obj.name);
    if(model != W_NULL)
    {
        wind_notice("device has been registered.\r\n");
        return W_ERR_FAIL;
    }
    wind_obj_init(&fsm_model->obj,WIND_FSM_MODEL_MAGIC,fsm_model->obj.name,&modellist);
    
    return W_ERR_OK;
}
w_err_t wind_fsm_model_unregister(w_fsm_model_s *fsm_model)
{
    w_err_t err;
    WIND_ASSERT_RETURN(fsm_model != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm_model->obj.magic == WIND_FSM_MODEL_MAGIC,W_ERR_INVALID);
    err = wind_obj_deinit(&fsm_model->obj,WIND_FSM_MODEL_MAGIC,&modellist);
    return err;
}

w_fsm_model_s *wind_fsm_model_get(char *name)
{
    return (w_fsm_model_s *)wind_obj_get(name,&modellist);
}

w_fsm_s *wind_fsm_create(char *name,w_int32_t id,char *modelname)
{
    w_err_t err;
    w_fsm_s *fsm = (w_fsm_s *)W_NULL;
    w_fsm_model_s *model;
    WIND_ASSERT_RETURN(name != W_NULL,(w_fsm_s *)W_NULL);
    WIND_ASSERT_RETURN(modelname != W_NULL,(w_fsm_s *)W_NULL);
    do
    {
        model = wind_fsm_model_get(modelname);
        WIND_ASSERT_BREAK(model != W_NULL,W_ERR_FAIL,"get fsm model:%s failed",modelname);
        fsm = fsm_malloc();
        WIND_ASSERT_RETURN(fsm != W_NULL,(w_fsm_s *)W_NULL);
        fsm->id = id;
        fsm->sleep_ms = 0;
        fsm->sleep_tick = 0;
        fsm->state = FSM_STAT_IDLE;
        fsm->cur_step = 0;
        fsm->arg = W_NULL;
        fsm->model= model;
        err = wind_obj_init(&fsm->obj,WIND_FSM_MAGIC,name,&fsmlist);
        
    }while(0);
    if(err != W_ERR_OK)
    {
        if(fsm != W_NULL)
            fsm_free(fsm);
        fsm = (w_fsm_s *)W_NULL;
    }
    return fsm;
}

w_err_t wind_fsm_destroy(w_fsm_s *fsm)
{
    w_err_t err;
    err = wind_obj_deinit(&fsm->obj,WIND_FSM_MAGIC,&fsmlist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    err = fsm_free(fsm);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

w_fsm_s *wind_fsm_get(char *name)
{
    return (w_fsm_s *)wind_obj_get(name,&fsmlist);
}

w_err_t wind_fsm_start(w_fsm_s *fsm)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->state = FSM_STAT_READY;
    return W_ERR_OK;
}
w_err_t wind_fsm_stop(w_fsm_s *fsm)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->state = FSM_STAT_STOP;
    fsm->cur_step = 0;
    return W_ERR_OK;
}

w_err_t wind_fsm_suspend(w_fsm_s *fsm)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->state = FSM_STAT_SUSPEND;
    return W_ERR_OK;
}

w_err_t wind_fsm_resume(w_fsm_s *fsm)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->state = FSM_STAT_READY;
    return W_ERR_OK;
}


w_err_t wind_fsm_sleep(w_fsm_s *fsm,w_int32_t time_ms)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->state = FSM_STAT_SLEEP;
    fsm->sleep_ms = time_ms;
    fsm->sleep_tick = wind_get_tick();
    return W_ERR_OK;
}

w_err_t wind_fsm_change_step(w_fsm_s *fsm,w_int32_t cur_step)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(cur_step < fsm->model->step_cnt,W_ERR_PTR_NULL);
    fsm->cur_step = cur_step;
    fsm->state = FSM_STAT_READY;
    return W_ERR_FAIL;
}

w_err_t wind_fsm_input(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    fsm->arg = arg;
    fsm->arglen = arglen; 
    fsm->state = FSM_STAT_READY;
    return W_ERR_FAIL;
}


w_err_t wind_fsm_schedule(void)
{
    w_fsm_s *fsm;
    w_err_t err;
    w_dnode_s *dnode;
    fsm_step_fn func;
    while(1)
    {
        fsm = get_reay_fsm();
        if(fsm != W_NULL)
            return W_ERR_OK;
        cur_fsm = fsm;
        func = fsm->model->steplist[fsm->cur_step].func;
        err = func(fsm->arg,fsm->arglen);
        if(err != W_ERR_OK)
        {
            dnode = cur_fsm->obj.objnode.next;
            cur_fsm = NODE_TO_FSM(dnode);
            err = wind_fsm_destroy(fsm);
            WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        }
    }
}


w_err_t wind_fsm_print(void)
{
    w_dnode_s *dnode;
    w_fsm_s *fsm;
    w_dlist_s *list = &fsmlist;
    wind_printf("\r\n\r\nfsm list:\r\n");
    wind_print_space(7);
    wind_printf("%-16s %-16s %-8s %-8s\r\n","fsm","mode","state","cur_step");
    wind_print_space(7);
    wind_disable_switch();
    foreach_node(dnode,list)
    {
        fsm = NODE_TO_FSM(dnode);
        wind_printf("%-16s %-16s %-8s\r\n",
            fsm->obj.name,fsm->model->obj.name,fsm_state_str(fsm->state),fsm->cur_step);
    }
    wind_enable_switch();
    wind_print_space(7);
    return W_ERR_OK;
}
#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
