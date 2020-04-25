/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: wind_fsm.h
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
#ifndef WIND_FSM_H__
#define WIND_FSM_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#if WIND_FSM_SUPPORT



/***********************************************macros*************************************************/
#define WIND_FSM_MODEL_MAGIC 0x26A451FE
#define WIND_FSM_MAGIC 0x3852FA95
typedef struct __w_fsm_s w_fsm_s;
typedef struct __fsm_step_s w_fsm_step_s;
typedef struct __w_fsm_model_s w_fsm_model_s;
#define F_FSM_POOL (0x01 << 0) //Mark whether the FSM object is allocated through the memory pool
#define IS_F_FSM_POOL(fsm) ((fsm->obj.flag & F_FSM_POOL) == F_FSM_POOL)
#define SET_F_FSM_POOL(fsm) (fsm->obj.flag |= F_FSM_POOL)
#define CLR_F_FSM_POOL(fsm) (fsm->obj.flag &= (~F_FSM_POOL))

#define FSM_STEP_START(fsm) w_fsm_step_s g_fsm_##fsm[] = {
#define FSM_STEP(step_id,func) {#step_id,step_id,func},
#define FSM_STEP_END };
#define FSM_MODEL_DEF(fsm) w_fsm_model_s g_fsmmodel_##fsm = {{WIND_FSM_MODEL_MAGIC,0,0,#fsm,W_NULL,W_NULL},sizeof(g_fsm_##fsm)/sizeof(w_fsm_step_s),g_fsm_##fsm};



/***********************************************enum*************************************************/
typedef enum
{
    FSM_STAT_IDLE,    //FSM idle state (initialization state)
    FSM_STAT_READY,   //FSM ready status
    FSM_STAT_SLEEP,   //FSM sleep state
    FSM_STAT_WAIT,    //FSM blocking status
    FSM_STAT_STOP,    //FSM stop status
}w_fsm_state_e;



/***********************************************struct*************************************************/

typedef w_err_t (*fsm_step_fn)(w_fsm_s *fsm);

struct __fsm_step_s
{
    char *step_name;   //FSM step name
    w_int32_t step_id; //FSM step index
    fsm_step_fn func;  //FSM step execution function
};

//FSM model,for building state machine objects
struct __w_fsm_model_s
{
    w_obj_s obj;            //Basic object information
    w_int32_t step_cnt;     //FSM step number
    w_fsm_step_s *steplist; //FSM step list
};

//FSM execution object
struct __w_fsm_s
{
    w_obj_s obj;           //Basic object information
    w_uint32_t sleep_cnt;  //FSM sleep time remaining (in milliseconds)
    w_uint32_t sleep_tick; //Tick value when FSM sleep
    w_fsm_state_e state;   //FSM state
    w_int32_t id;          //FSM object ID
    w_int16_t cur_step;    //FSM current step ID
    w_int16_t sub_step;    //FSM subscript step ID
    void      *arg;        //FSM input parameter
    w_int32_t arglen;      //FSM input parameter lenth
    w_mutex_s mutex;       //FSM mutex lock
    w_fsm_model_s *model;  //Model of the FSM object
};


/********************************************global variable declare**********************************************/



/********************************************global function declare**********************************************/
w_err_t _wind_fsm_mod_init(void);

w_err_t wind_fsm_model_register(w_fsm_model_s *fsm_model);
w_err_t wind_fsm_model_unregister(w_fsm_model_s *fsm_model);
w_fsm_model_s *wind_fsm_model_get(char *name);

w_err_t wind_fsm_init(w_fsm_s *fsm,char *name,w_int32_t id,char *modelname);
w_fsm_s *wind_fsm_create(char *name,w_int32_t id,char *modelname);
w_err_t wind_fsm_destroy(w_fsm_s *fsm);
w_fsm_s *wind_fsm_get(char *name);

w_err_t wind_fsm_start(w_fsm_s *fsm);
w_err_t wind_fsm_stop(w_fsm_s *fsm);
w_err_t wind_fsm_wait(w_fsm_s *fsm);
w_err_t wind_fsm_input(w_fsm_s *fsm,void *arg,w_int32_t arglen);
w_err_t wind_fsm_sleep(w_fsm_s *fsm,w_int32_t time_ms);
w_err_t wind_fsm_wakeup(w_fsm_s *fsm);
w_err_t wind_fsm_change_step(w_fsm_s *fsm,w_int32_t new_step);
//w_err_t wind_fsm_run(w_fsm_s *fsm);
w_err_t wind_fsm_print(void);


#endif // #if WIND_FSM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_FSM_H__
