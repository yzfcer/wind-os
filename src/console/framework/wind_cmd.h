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
#include "console_framework.h"
#ifdef __cplusplus
extern "C" {
#endif

#define console_printf(fmt,...) wind_printf(fmt,##__VA_ARGS__)
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

