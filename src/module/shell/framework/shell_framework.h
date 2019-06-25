/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
** �?  �?  �? shell_framework.h / shell_framework.c
** �?  �?  �? Jason Zhou
** 最后修改日�? 2015/1/24 20:08:03
** �?       �? 能够支持标准输入输出的控制台的框�?
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建�? Jason Zhou
** �? �? v1.0
** 日　�? 2015/1/24 20:08:03
** 描　�? 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改�? 
** 日　�? 
** 描　�? 
** 本文件由C语言源文件模板软件生成�?-----------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __SHELL_FRAMEWORK_H__
#define __SHELL_FRAMEWORK_H__
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

/*********************************************头文件定�?**********************************************/

#if WIND_SHELL_SUPPORT

/***********************************************宏定�?************************************************/
#define WIND_CMD_MAX_LEN 512//一个命令的最大长�?
#define WIND_CMD_NAME_LEN 12//一个命令标示的最大长�?
#define WIND_CTL_USRNAME_LEN 20//用户名的长度
#define WIND_CTL_PWD_LEN 20//密码的最大长�?
#define WIND_SHELL_CTX_COUNT 1//支持的控制套终端的数�?
#define CMD_PARAM_CNT 10

#define USER_AUTH_ENABLE 0
#define USER_AUTH_ERR_MAX 5
#define USER_AUTH_WAIT_SEC 60
    


/**********************************************枚举定义************************************************/





/*********************************************结构体定�?**********************************************/

typedef enum __w_shstat_e
{
    CSLSTAT_USER,//需要输入用户名
    CSLSTAT_PWD,//需要输入密�?
    CSLSTAT_CMD,//在命令行模式
    CSLSTAT_APP //运行于应用程序模�?暂时无用
} w_shstat_e;

typedef struct __w_cmd_s
{
    w_dnode_s cmdnode;
    char* name;//命令的名�?
    void (*showdisc)(void);//简要功能说�?
    void (*showusage)(void);//详细的帮助说�?
    w_err_t (*execute)(w_int32_t argc,char **argv);//命令的入口函�?
}w_cmd_s;


//得到分解后的参数列表
typedef struct __cmd_param_s
{
    w_uint32_t argc;
    char *argv[CMD_PARAM_CNT];
}w_cmd_param_s;


typedef struct __w_shell_ctx_s
{
    w_shstat_e stat;//当前的解析状�?
    w_int32_t index;//命令的下一个字符下�?
    w_int8_t key_evt_f;
    w_int8_t key_evt_len;
    w_uint16_t key_value;
    w_uint32_t key_evt_id;
    w_int8_t autherr_cnt;
    char buf[WIND_CMD_MAX_LEN];//接收的数据缓存区
    char user[WIND_CTL_USRNAME_LEN];//用户�?
    char passwd[WIND_CTL_PWD_LEN];//密码的�?
    
    w_cmd_his_s his;
    w_cmd_param_s param;
    w_dlist_s cmd_list;
}w_shell_ctx_s;


/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/

//输出命令列表
void wind_cmd_init(w_shell_ctx_s *ctx);
w_cmd_s *wind_cmd_get(const char *name);
w_err_t wind_cmd_register(w_cmd_s *cmd,int cnt);
w_err_t wind_cmd_unregister(w_cmd_s *cmd);
w_err_t wind_cmd_print(void);
void _wind_register_all_cmd(w_shell_ctx_s *ctx);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
