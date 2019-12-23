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
#ifndef __WIND_FSM_H__
#define __WIND_FSM_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_debug.h"



/***********************************************宏定义*************************************************/
#define WIND_FSM_MODEL_MAGIC 0x26A451FE
#define WIND_FSM_MAGIC 0x3852FA95

#define FSM_STEP_START(fsm) w_fsm_step_s g_fsm_##fsm[] = {
#define FSM_STEP(step_id,func) {#step_id,step_id,func},
#define FSM_STEP_END };



/**********************************************枚举定义************************************************/
typedef enum
{
    FSM_STAT_IDLE,
    FSM_STAT_READY,
    FSM_STAT_SLEEP,
    FSM_STAT_SUSPEND,
    FSM_STAT_STOP,
}fsm_state_e;



/*********************************************结构体定义***********************************************/

typedef w_err_t (*fsm_step_fn)(void *arg,w_int32_t arglen);

typedef struct __fsm_step_s
{
    char *step_name;
    w_int32_t step_id;
    fsm_step_fn func;
}w_fsm_step_s;

typedef struct 
{
    w_obj_s obj;
    w_int32_t step_cnt;
    w_fsm_step_s *steplist;
}w_fsm_model_s;

typedef struct __w_fsm_s
{
    w_obj_s obj;
    w_int32_t id;
    w_uint32_t sleep_ms;
    w_uint32_t sleep_tick;
    fsm_state_e state;
    w_int32_t cur_step;
    void *arg;
    w_int32_t arglen;
    w_fsm_model_s *model;
}w_fsm_s;


/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/
w_err_t _wind_fsm_mod_init(void);

w_err_t wind_fsm_model_register(w_fsm_model_s *fsm_model);
w_err_t wind_fsm_model_unregister(w_fsm_model_s *fsm_model);
w_fsm_model_s *wind_fsm_model_get(char *name);

w_fsm_s *wind_fsm_create(char *name,w_int32_t id,char *modelname);
w_err_t wind_fsm_destroy(w_fsm_s *fsm);
w_fsm_s *wind_fsm_get(char *name);

w_err_t wind_fsm_start(w_fsm_s *fsm);
w_err_t wind_fsm_stop(w_fsm_s *fsm);
w_err_t wind_fsm_suspend(w_fsm_s *fsm);
w_err_t wind_fsm_sleep(w_fsm_s *fsm,w_int32_t time_ms);
w_err_t wind_fsm_change_step(w_fsm_s *fsm,w_int32_t cur_step);
w_err_t wind_fsm_input(w_fsm_s *fsm,void *arg,w_int32_t arglen);
w_err_t wind_fsm_schedule(void);
w_err_t wind_fsm_print(void);




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __WIND_FSM_H__
