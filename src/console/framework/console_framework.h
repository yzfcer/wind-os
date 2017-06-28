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
#include "wind_types.h"
#include "wind_err.h"
#include "wind_key.h"
#include "wind_console.h"
#include "wind_assert.h"
#include "wind_string.h"
#include "wind_thread.h"

/***********************************************宏定义*************************************************/
#define WIND_CMD_MAX_LEN 512//一个命令的最大长度
#define WIND_CMD_NAME_LEN 12//一个命令标示的最大长度
#define CMD_PARAM_CNT 10
#define WIND_CTL_USRNAME_LEN 20//用户名的长度
#define WIND_CTL_PWD_LEN 20//密码的最大长度
#define WIND_CONSOLE_COUNT 1//支持的控制套终端的数量
#define CONSOLE_OUT(...) wind_printf(__VA_ARGS__)


/**********************************************枚举定义************************************************/





/*********************************************结构体定义***********************************************/




//全局的test suite列表
typedef struct __cmd_global
{
    cmd_s *head;
    cmd_s *tail;
    u32_t cnt;
}cmd_global_s;

//得到分解后的参数列表
typedef struct __cmd_param_s
{
    u32_t cnt;
    char * param[CMD_PARAM_CNT];
}cmd_param_s;
typedef struct __console_s
{
    char cmdstr[WIND_CMD_MAX_LEN];//命令的缓存区
    char buf[WIND_CMD_MAX_LEN];//接收的数据缓存区
    u32_t index;//命令的下一个字符下标
    cslstat_e stat;//当前的解析状态
    char user[WIND_CTL_USRNAME_LEN];//用户名
    char pwd[WIND_CTL_PWD_LEN];//密码的值
    cmd_param_s param;
    cmd_global_s cmd_list;
}console_s;
/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/

void register_cmd_echo(console_s *ctrl);
void register_cmd_help(console_s *ctrl);
void register_cmd_proc(console_s *ctrl);
void register_cmd_show(console_s *ctrl);
void register_cmd_stat(console_s *ctrl);
void register_cmd_test(console_s *ctrl);



//输出命令列表
err_t wind_output_cmdlist(void);

void console_framework_init(console_s *ctlobj);
cmd_s *wind_get_cmdlist(void);
s32_t is_string_equal(char *dest,char *src);
err_t wind_cmd_register(cmd_global_s *cgl,cmd_s *cmd,int cnt);
//err_t consoleProc(s32_t argc,char **argv);
pthread_s lunch_console(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
