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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_key.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "cmd_history.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************头文件定义***********************************************/

#if WIND_CONSOLE_SUPPORT

/***********************************************宏定义*************************************************/
#define WIND_CMD_MAX_LEN 512//一个命令的最大长度
#define WIND_CMD_NAME_LEN 12//一个命令标示的最大长度
#define WIND_CTL_USRNAME_LEN 20//用户名的长度
#define WIND_CTL_PWD_LEN 20//密码的最大长度
#define WIND_CONSOLE_COUNT 1//支持的控制套终端的数量
#define CMD_PARAM_CNT 10

#define USER_AUTHENTICATION_EN 0
    


/**********************************************枚举定义************************************************/





/*********************************************结构体定义***********************************************/

typedef enum __cslstat_e
{
    CSLSTAT_USER,//需要输入用户名
    CSLSTAT_PWD,//需要输入密码
    CSLSTAT_CMD,//在命令行模式
    CSLSTAT_APP //运行于应用程序模式,暂时无用
} cslstat_e;

typedef struct __cmd_s
{
    //struct __cmd_s *next;
    dnode_s cmdnode;
    char* name;//命令的名称
    void (*showdisc)(void);//简要功能说明
    void (*showusage)(void);//详细的帮助说明
    w_err_t (*execute)(w_int32_t argc,char **argv);//命令的入口函数
}cmd_s;


#if 0
//全局的cmd列表
typedef struct __cmd_list
{
    cmd_s *head;
    cmd_s *tail;
    w_uint32_t cnt;
}cmd_list_s;
#endif


//得到分解后的参数列表
typedef struct __cmd_param_s
{
    w_uint32_t argc;
    char *argv[CMD_PARAM_CNT];
}cmd_param_s;


typedef struct __console_s
{
    cslstat_e stat;//当前的解析状态
    w_int32_t index;//命令的下一个字符下标
    w_int8_t key_evt_f;
    w_int8_t key_evt_len;
    w_uint32_t key_evt_id;
    char buf[WIND_CMD_MAX_LEN];//接收的数据缓存区
    char user[WIND_CTL_USRNAME_LEN];//用户名
    char pwd[WIND_CTL_PWD_LEN];//密码的值
    
    cmd_his_s his;
    cmd_param_s param;
    dlist_s cmd_list;
}console_s;


/********************************************全局变量申明**********************************************/



/********************************************全局函数申明**********************************************/

//输出命令列表
void wind_cmd_init(console_s *ctrl);
w_err_t _create_console_thread(void);
cmd_s *wind_cmd_get(const char *name);
w_err_t wind_cmd_register(cmd_s *cmd,int cnt);
w_err_t wind_cmd_print(void);
void _wind_register_all_cmd(console_s *ctrl);
extern void wind_cmd_register_cmd_test(console_s *ctrl);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__
