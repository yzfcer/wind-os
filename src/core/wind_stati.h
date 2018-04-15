/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_stat.h
**创   建   人: 周江村
**最后修改日期: wind os的资源统计相关的代码
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.08.13
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STAT_H__
#define WIND_STAT_H__
#include "wind_type.h"
#include "wind_config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum __objid_e
{
    IDX_THREAD,
    IDX_LOCK,
#if WIND_SEM_SUPPORT
    IDX_SEM,
#endif
#if WIND_PIPE_SUPPORT
    IDX_PIPE,
#endif
#if WIND_MSGBOX_SUPPORT
    IDX_MSG,
    IDX_MBOX,
#endif
#if WIND_TIMER_SUPPORT
    IDX_TIMER,
#endif
    IDX_STACK,
    IDX_CNT
}obj_id_e;


typedef struct __stat_s
{
    w_uint32_t tot;
    w_uint32_t used;
    w_uint32_t max;
    w_uint32_t err;
}stati_s,*pstati_s;

#define WIND_STATI_INIT(stati,totel) \
            do{stati.tot = totel;\
            stati.used = 0;\
            stati.max = 0;\
            stati.err = 0;}while(0)
#define WIND_STATI_INC(stati) do{stati.used++;WIND_STATI_MAX(stati);}while(0)
#define WIND_STATI_MINUS(stati) do{if(stati.used > 0) stati.used--;}while(0)
#define WIND_STATI_ERR_INC(stati) stati.err++
#define WIND_STATI_MAX(stati) \
        do{if(stati.used > stati.max) \
        stati.max = stati.used;}while(0)

#ifdef __cplusplus
}
#endif


#endif

