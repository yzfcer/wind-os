/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : wind_fsm.c
** Author      : Jason Zhou
** Last Date   : 2019/12/1 15:34:56
** Description : Functional model framework of FSM
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019/12/1 15:34:56
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_fsm.h"
#include "wind_core.h"
#include "wind_debug.h"
#include "wind_obj.h"
#include "wind_pool.h"
#if WIND_FSM_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define NODE_TO_FSM(node) NODEPTR_TO_ITEMPTR(node,w_fsm_s,obj.objnode)
#define NODE_TO_FSM_MODEL(node) NODEPTR_TO_ITEMPTR(node,w_fsm_model_s,obj.objnode)

/********************************************internal variables**********************************************/
static w_dlist_s modellist;
static w_dlist_s fsmlist;
static WIND_POOL(fsmpool,WIND_FSM_MAX_NUM,sizeof(w_fsm_s));

/********************************************internal functions**********************************************/
static w_fsm_s *fsm_malloc(void)
{
    return (w_fsm_s *)wind_pool_malloc(fsmpool);
}

static w_err_t fsm_free(w_fsm_s *fsm)
{
    return wind_pool_free(fsmpool,fsm);
}


static char *fsm_state_str(w_fsm_state_e state)
{
    if(state == FSM_STAT_IDLE)
        return "idle";
    else if(state == FSM_STAT_READY)
        return "ready";
    else if(state == FSM_STAT_SLEEP)
        return "sleep";
    else if(state == FSM_STAT_WAIT)
        return "wait";
    else if(state == FSM_STAT_STOP)
        return "stop";
    return "unknown";
}

static w_err_t wind_fsm_run(w_fsm_s *fsm)
{
    w_err_t err;
    fsm_step_fn func;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    err = W_ERR_OK;
    wind_mutex_lock(&fsm->mutex);
    while(fsm->state == FSM_STAT_READY)
    {
        func = fsm->model->steplist[fsm->cur_step].func;
        err = func(fsm);
        if(err != W_ERR_OK)
        {
            break;
        }
    }
    wind_mutex_unlock(&fsm->mutex);
    return err;
}

/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
w_err_t _wind_fsm_mod_init(void)
{
    w_err_t err;
    DLIST_INIT(modellist);
    DLIST_INIT(fsmlist);
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

w_err_t wind_fsm_model_print_list(void)
{
    return wind_obj_print_list(&modellist);
}

w_err_t wind_fsm_init(w_fsm_s *fsm,char *name,w_int32_t id,char *modelname)
{
    w_err_t err;
    w_fsm_model_s *model;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(modelname != W_NULL,W_ERR_PTR_NULL);
    do
    {
        model = wind_fsm_model_get(modelname);
        WIND_ASSERT_BREAK(model != W_NULL,W_ERR_FAIL,"get fsm model:%s failed",modelname);
        fsm->sleep_cnt = 0;
        fsm->sleep_tick = 0;
        fsm->state = FSM_STAT_IDLE;
        fsm->id = id;
        fsm->cur_step = 0;
        fsm->sub_step = 0;
        fsm->arg = W_NULL;
        fsm->model= model;
        err = wind_mutex_init(&fsm->mutex,name);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"init fsm mutex failed");
        err = wind_obj_init(&fsm->obj,WIND_FSM_MAGIC,name,&fsmlist);
    }while(0);
    return err;
}

w_fsm_s *wind_fsm_create(char *name,w_int32_t id,char *modelname)
{
    w_err_t err;
    w_fsm_s *fsm = (w_fsm_s *)W_NULL;
    
    WIND_ASSERT_RETURN(name != W_NULL,(w_fsm_s *)W_NULL);
    WIND_ASSERT_RETURN(modelname != W_NULL,(w_fsm_s *)W_NULL);
    do
    {
        fsm = fsm_malloc();
        WIND_ASSERT_RETURN(fsm != W_NULL,(w_fsm_s *)W_NULL);
        err = wind_fsm_init(fsm,name,id,modelname);
        if(err == W_ERR_OK)
            SET_F_FSM_POOL(fsm);
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
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    do
    {
        err = wind_mutex_destroy(&fsm->mutex);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"destroy fsm mutex failed\r\n");
        err = wind_obj_deinit(&fsm->obj,WIND_FSM_MAGIC,&fsmlist);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"deinit fsm obj failed\r\n");
        if(IS_F_FSM_POOL(fsm))
            fsm_free(fsm);
    }while(0);
    return err;
}

w_fsm_s *wind_fsm_get(char *name)
{
    return (w_fsm_s *)wind_obj_get(name,&fsmlist);
}

w_err_t wind_fsm_start(w_fsm_s *fsm)
{
    w_err_t err;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(fsm->state == FSM_STAT_IDLE,W_ERR_FAIL,"fsm state must be idle");
        fsm->state = FSM_STAT_READY;
        wind_fsm_run(fsm);
    }while(0);
    wind_mutex_unlock(&fsm->mutex);
    return err;
}

w_err_t wind_fsm_stop(w_fsm_s *fsm)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    fsm->state = FSM_STAT_STOP;
    fsm->cur_step = 0;
    wind_mutex_unlock(&fsm->mutex);
    return W_ERR_OK;
}

w_err_t wind_fsm_wait(w_fsm_s *fsm)
{
    w_err_t err;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(fsm->state == FSM_STAT_READY,W_ERR_FAIL,"fsm state error");
        fsm->state = FSM_STAT_WAIT;
        wind_mutex_unlock(&fsm->mutex);
    }while(0);
    return err;
}

w_err_t wind_fsm_input(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    w_err_t err;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(fsm->state == FSM_STAT_WAIT,W_ERR_FAIL,"fsm state error");
        fsm->arg = arg;
        fsm->arglen = arglen; 
        fsm->state = FSM_STAT_READY;
        wind_fsm_run(fsm);
    }while(0);
    wind_mutex_unlock(&fsm->mutex);
    return err;
}




w_err_t wind_fsm_sleep(w_fsm_s *fsm,w_int32_t time_ms)
{
    w_err_t err;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(fsm->state == FSM_STAT_READY,W_ERR_FAIL,"fsm state error");
        fsm->state = FSM_STAT_SLEEP;
        fsm->sleep_cnt = time_ms * (1000 / WIND_TICK_PER_SEC);
        fsm->sleep_tick = wind_get_tick();
    }while(0);
    wind_mutex_unlock(&fsm->mutex);
    return err;
}

w_err_t wind_fsm_wakeup(w_fsm_s *fsm)
{
    w_err_t err;
    w_uint32_t tick;
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    wind_mutex_lock(&fsm->mutex);
    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(fsm->state == FSM_STAT_SLEEP,W_ERR_FAIL,"fsm state error");
        tick = wind_get_tick();
        tick -= fsm->sleep_tick;
        WIND_CHECK_BREAK(tick >= fsm->sleep_cnt,W_ERR_FAIL);
        fsm->state = FSM_STAT_READY;
        wind_fsm_run(fsm);
    }while(0);
    wind_mutex_unlock(&fsm->mutex);
    return err;
}

w_err_t wind_fsm_change_step(w_fsm_s *fsm,w_int32_t new_step)
{
    WIND_ASSERT_RETURN(fsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(fsm->obj.magic == WIND_FSM_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(new_step < fsm->model->step_cnt,W_ERR_PTR_NULL);
    wind_mutex_lock(&fsm->mutex);
    wind_debug("fsm %s goto step %s",wind_obj_name(&fsm->obj),
                fsm->model->steplist[new_step]);
	fsm->cur_step = (w_int16_t)new_step;
    fsm->state = FSM_STAT_READY;
    wind_mutex_unlock(&fsm->mutex);
    return W_ERR_OK;
}




w_err_t wind_fsm_print_list(void)
{
    return wind_obj_print_list(&fsmlist);
}


w_err_t wind_fsm_print_detail(void)
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

#endif // #if WIND_FSM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
