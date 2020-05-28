/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : shell_framework.h
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : Shell module basic framework header
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef SHELL_FRAMEWORK_H__
#define SHELL_FRAMEWORK_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_key.h"
#include "wind_string.h"
#include "cmd_history.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_MODULE_SHELL_SUPPORT

#define WIND_CMD_MAX_LEN 512//
#define WIND_CMD_NAME_LEN 12//
#define WIND_CTL_USRNAME_LEN 20//
#define WIND_CTL_PWD_LEN 20//
#define WIND_SHELL_CTX_COUNT 1//
#define CMD_PARAM_CNT 10

#define USER_AUTH_ENABLE 0
#define USER_AUTH_ERR_MAX 5
#define USER_AUTH_WAIT_SEC 60

typedef enum __w_shstat_e
{
    CSLSTAT_USER,//
    CSLSTAT_PWD,//
    CSLSTAT_CMD,//
    CSLSTAT_APP //
} w_shstat_e;

typedef struct __w_cmd_s
{
    w_dnode_s cmdnode;
    char* name;//
    void (*showdisc)(void);//
    void (*showusage)(void);//
    w_err_t (*execute)(w_int32_t argc,char **argv);//
}w_cmd_s;



typedef struct __cmd_param_s
{
    w_uint32_t argc;
    char *argv[CMD_PARAM_CNT];
}w_cmd_param_s;


typedef struct __w_shell_ctx_s
{
    w_shstat_e stat;//
    w_int32_t index;//
    w_int8_t key_evt_f;
    w_int8_t key_evt_len;
    w_uint16_t key_value;
    w_uint32_t key_evt_id;
    w_int8_t autherr_cnt;
    char buf[WIND_CMD_MAX_LEN];//
    char user[WIND_CTL_USRNAME_LEN];//
    char passwd[WIND_CTL_PWD_LEN];//
    
    w_cmd_his_s his;
    w_cmd_param_s param;
    w_dlist_s cmd_list;
}w_shell_ctx_s;



w_err_t wind_cmd_init(void);
w_cmd_s *wind_cmd_get(const char *name);
w_err_t wind_cmd_register(w_cmd_s *cmd);
w_err_t wind_cmd_unregister(w_cmd_s *cmd);
w_err_t wind_cmd_print(void);
void _wind_register_all_cmd(void);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef SHELL_FRAMEWORK_H__