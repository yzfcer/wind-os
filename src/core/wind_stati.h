/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_stati.h
** Author      : Jason Zhou
** Last Date   : wind-os definition of resource statistics
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.08.13
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STAT_H__
#define WIND_STAT_H__
#include "wind_type.h"
#include "wind_config.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


typedef struct __stat_s
{
    w_uint32_t tot;
    w_uint32_t used;
    w_uint32_t max;
    w_uint32_t err;
}w_stati_s;

#define WIND_STATI_INIT(stati,totel) \
            do{stati.tot = totel;\
            stati.used = 0;\
            stati.max = 0;\
            stati.err = 0;}while(0)
#define WIND_STATI_INC(stati) do{stati.used++;WIND_STATI_MAX(stati);}while(0)
#define WIND_STATI_DEC(stati) do{if(stati.used > 0) stati.used--;}while(0)
#define WIND_STATI_ERR_INC(stati) stati.err++
#define WIND_STATI_MAX(stati) \
        do{if(stati.used > stati.max) \
        stati.max = stati.used;}while(0)
#define WIND_STATI_ADD(stati,num) do{stati.used+=num;WIND_STATI_MAX(stati);}while(0)
#define WIND_STATI_MINUS(stati,num) do{if(stati.used >= (num)) stati.used-=(num);}while(0)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_STAT_H__

