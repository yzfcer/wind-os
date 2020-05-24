/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_user.h
** Author      : Jason Zhou
** Last Date   : 2019.01.28
** Description : 系统用户管理模块
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.01.28
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.01.28
** Description : 
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
#endif // #ifdef __cplusplus

#if WIND_USER_SUPPORT
#define USER_NAME_MAXLEN 32
#define PASSWD_MAXLEN 32

#define WIND_USER_MAGIC 0x3A6372A9

#define F_USER_POOL (0x01 << 0) //Mark whether the user object is allocated through the memory pool
#define IS_F_USER_POOL(user) ((user->flag & F_USER_POOL) == F_USER_POOL)
#define SET_F_USER_POOL(user) (user->flag |= F_USER_POOL)
#define CLR_F_USER_POOL(user) (user->flag &= (~F_USER_POOL))

typedef enum
{
    USER_SUPER = 0, //Super user
    USER_ADMIN = 1, //Administrator user
    USER_COMMON = 2,//Common users
}w_user_e;

typedef struct _wind_user
{
    w_uint32_t magic;            //Magic code
    char name[USER_NAME_MAXLEN]; //User name
    char passwd[PASSWD_MAXLEN];  //User password
    w_uint16_t flag;             //User Attribute tag
    w_user_e usertype;           //user type
    w_dnode_s usernode;          //Node for adding user to user list 
}w_user_s;

w_err_t _wind_user_mod_init(void);
w_user_s *wind_user_get(const char *name);
w_err_t wind_user_init(w_user_s *user,w_user_e usertype,const char *username,const char *passwd);
w_user_s *wind_user_create(w_user_e usertype,const char *username,const char *passwd);
w_err_t wind_user_destroy(w_user_s *user);
w_err_t wind_user_modify_passwd(w_user_s *user,const char *newpasswd);
w_err_t wind_user_print(void);

#endif // #if WIND_USER_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif// #ifndef WIND_USER_H__

