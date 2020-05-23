/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: module_shell.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: shell模块入口
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/1/24 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
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