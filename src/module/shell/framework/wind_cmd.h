/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**�?  �?  �? wind_cmd.h
**�?  �?  �? Jason Zhou
**最后修改日�? 2012.09.26
**�?       �? 系统的控制台命令定义�?
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建�? Jason Zhou
** �? �? v1.0
** 日　�? 2013.06.26
** 描　�? 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改�? Jason Zhou
** 日　�? 2012.06.26
** 描　�? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CONSOLE_H__
#define WIND_CONSOLE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "shell_framework.h"
#if WIND_MODULE_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_DISC(cmdname) static void cmd_##cmdname##_disc(void)
#define COMMAND_USAGE(cmdname) static void cmd_##cmdname##_help(void)
#define COMMAND_MAIN(cmdname,argc,argv) static w_err_t cmd_##cmdname##_main(w_int32_t argc,char **argv)
#define COMMAND_DEF(cmdname) w_cmd_s g_cmd_##cmdname = { \
{W_NULL,W_NULL},#cmdname,cmd_##cmdname##_disc,\
cmd_##cmdname##_help,cmd_##cmdname##_main}

#define COMMAND_DECLARE(cmdname) extern w_cmd_s g_cmd_##cmdname
#define COMMAND(cmdname) &g_cmd_##cmdname

#ifdef __cplusplus
}
#endif
#endif
#endif//ifndef WIND_CONSOLE_H__

