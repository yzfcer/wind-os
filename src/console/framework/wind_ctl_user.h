/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_user.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统的用户管理相关的定义
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_USER_H__
#define WIND_USER_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

#define WIND_USER_NAME_LEN 20
#define WIND_USER_PWD_LEN 20

typedef enum __usrprio_e
{
    USRPRIO_SUPPER,//超级管理员
    USRPRIO_MGR,//一般管理员
    USRPRIO_COMMAN//普通用户
}usrprio_e;

typedef struct __user_s
{
    //struct __user_s *next;
    w_int8_t name[WIND_USER_NAME_LEN];
    w_int8_t pwd[WIND_USER_PWD_LEN];
    usrprio_e prio;
} user_s;


user_s *wind_user_create(w_int8_t *name,w_int8_t *pwd,usrprio_e prio);
w_err_t wind_user_insert(user_s *usr);
w_err_t wind_user_remove(user_s *usr);





#ifdef __cplusplus
}
#endif
#endif
#endif
