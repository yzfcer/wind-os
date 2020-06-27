/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : wind_cmd.h
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : Test framework user header file
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

typedef struct
{
    char *keyword;
    w_err_t (*exec)(w_int32_t argc,char **argv);
}w_cmd_handle_s;

#define COMMAND_DISC(cmdname) static void cmd_##cmdname##_disc(void)
#define COMMAND_USAGE(cmdname) static void cmd_##cmdname##_help(void)
#define COMMAND_MAIN(cmdname,argc,argv) static w_err_t cmd_##cmdname##_main(w_int32_t argc,char **argv)
#define COMMAND_DEF(cmdname) w_cmd_s g_cmd_##cmdname = { \
{(w_dnode_s*)W_NULL,(w_dnode_s*)W_NULL},#cmdname,cmd_##cmdname##_disc,\
cmd_##cmdname##_help,cmd_##cmdname##_main}

#define COMMAND_DECLARE(cmdname) extern w_cmd_s g_cmd_##cmdname
#define COMMAND(cmdname) &g_cmd_##cmdname

#ifdef __cplusplus
}
#endif
#endif
#endif//ifndef WIND_CONSOLE_H__

