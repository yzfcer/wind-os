/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: console_framework.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 系统的控制台入口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.06.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.06.26
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CONSOLE_H__
#define WIND_CONSOLE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"

#ifdef __cplusplus
extern "C" {
#endif
#define WIND_CTL_TEST_EN 1//控制台功能的时能标志
#define WIND_CTL_DEBUG_EN 0//控制台调试时能标志

#define WIND_CMD_MAX_LEN 512//一个命令的最大长度
#define WIND_CMD_NAME_LEN 12//一个命令标示的最大长度
#define WIND_CTL_USRNAME_LEN 20//用户名的长度
#define WIND_CTL_PWD_LEN 20//密码的最大长度
#define WIND_CONSOLE_COUNT 1//支持的控制套终端的数量




typedef enum __cslstat_e
{
    CSLSTAT_USER,//需要输入用户名
    CSLSTAT_PWD,//需要输入密码
    CSLSTAT_CMD,//在命令行模式
    CSLSTAT_APP //运行于应用程序模式
} cslstat_e;

typedef struct __cmd_s
{
    struct __cmd_s *next;
    char* cmd;//命令的名称
    void (*showdisc)(void);//简要功能说明
    void (*showusage)(void);//详细的帮助说明
    w_err_t (*execute)(w_int32_t argc,char **argv);//命令的入口函数
}cmd_s,*pcmd_s;

#define CMD_DECLARE(CMD) extern cmd_s g_cmd_##CMD

#define COMMAND(CMD) &g_cmd_##CMD

#define CMD_DEF(CMD) \
cmd_s g_cmd_##CMD = { \
NULL,#CMD,cmd_showdisc,\
cmd_showusage,cmd_main}



#endif//ifndef WIND_CONSOLE_H__
#ifdef __cplusplus
}
#endif

