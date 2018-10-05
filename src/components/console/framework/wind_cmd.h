/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_cmd.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ϵͳ�Ŀ���̨������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.06.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.06.26
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CONSOLE_H__
#define WIND_CONSOLE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "console_framework.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

#define console_printf(fmt,...) wind_printf(fmt,##__VA_ARGS__)

#define COMMAND_DISC(cmdname) static void cmd_##cmdname##_disc(void)
#define COMMAND_USAGE(cmdname) static void cmd_##cmdname##_help(void)
#define COMMAND_MAIN(cmdname,argc,argv) static w_err_t cmd_##cmdname##_main(w_int32_t argc,char **argv)
#define COMMAND_DEF(cmdname) w_cmd_s g_cmd_##cmdname = { \
{W_NULL,W_NULL},#cmdname,cmd_##cmdname##_disc,\
cmd_##cmdname##_help,cmd_##cmdname##_main}

#define CMD_DECLARE(cmdname) extern w_cmd_s g_cmd_##cmdname
#define COMMAND(cmdname) &g_cmd_##cmdname

#ifdef __cplusplus
}
#endif
#endif
#endif//ifndef WIND_CONSOLE_H__

