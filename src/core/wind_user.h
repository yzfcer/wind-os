/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_user.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.28
**描        述: 系统用户管理模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.28
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.28
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_USER_H__
#define WIND_USER_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_USER_SUPPORT
#define USER_NAME_MAXLEN 32
#define PASSWD_MAXLEN 32

#define WIND_USER_MAGIC 0x3A6372A9
typedef enum
{
    USER_SUPER = 0,
    USER_ADMIN = 1,
    USER_COMMON = 2,
}w_user_e;

typedef struct _wind_user
{
    w_uint32_t magic;
    char name[USER_NAME_MAXLEN];
    char passwd[PASSWD_MAXLEN];
    w_user_e usertype;
    w_uint32_t flag_pool:1;
    w_dnode_s usernode;
}w_user_s;

w_err_t _wind_user_mod_init(void);
w_user_s *wind_user_get(const char *name);
w_err_t wind_user_init(w_user_s *user,w_user_e usertype,const char *username,const char *passwd);
w_user_s *wind_user_create(w_user_e usertype,const char *username,const char *passwd);
w_err_t wind_user_destroy(w_user_s *user);
w_err_t wind_user_modify_passwd(w_user_s *user,const char *newpasswd);
w_err_t wind_user_print(void);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_USER_H__

