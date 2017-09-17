/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: console_framework.h / console_framework.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 20:08:03
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/1/24 20:08:03
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 能够支持多个标准输入输出的控制台的框架
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __CONSOLE_FRAMEWORK_H__
#define __CONSOLE_FRAMEWORK_H__
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_err.h"
#include "wind_key.h"
#include "wind_console.h"
#include "wind_assert.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "cmd_history.h"
/***********************************************宏定义*************************************************/
#define WIND_CMD_MAX_LEN 512//一个命令的最大长度
#define WIND_CMD_NAME_LEN 12//一个命令标示的最大长度
#define CMD_PARAM_CNT 10
#define WIND_CTL_USRNAME_LEN 20//用户名的长度
#define WIND_CTL_PWD_LEN 20//密码的最大长度
#define WIND_CONSOLE_COUNT 1//支持的控制套终端的数量


/**********************************************枚举定义************************************************/





/*********************************************结构体定义***********************************************/




//全局的cmd列表
typedef struct __cmd_list
{
    cmd_s *head;
    cmd_s *tail;
    w_uint32_t cnt;
}cmd_list_s;



//得到分解后的参数列表
typedef struct __cmd_param_s
{
    w_uint32_t argc;
    char * argv[CMD_PARAM_CNT];
}cmd_param_s;

typedef struct 
{
    w_err_t (*getchar)(char *ch);
    w_int32_t (*printf)(const char *fmt,...);
}console_ops;


typedef struct __console_s
{
    cslstat_e stat;//当前的解析状态
    w_int32_t index;//命令的下一个字符下标
    char buf[WIND_CMD_MAX_LEN];//接收的数据缓存区
    char user[WIND_CTL_USRNAME_LEN];//用户名
    char pwd[WIND_CTL_PWD_LEN];//密码的值
    cmd_his_s his;
    cmd_param_s param;
    cmd_list_s cmd_list;
    console_ops ops;
}console_s;
/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/

//输出命令列表
w_err_t wind_output_cmdlist(void);

void console_framework_init(console_s *ctlobj);
cmd_s *wind_get_cmdlist(void);
w_err_t wind_cmd_register(cmd_list_s *cgl,cmd_s *cmd,int cnt);
void create_console_thread(void);
void register_all_cmd(console_s *ctrl);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
