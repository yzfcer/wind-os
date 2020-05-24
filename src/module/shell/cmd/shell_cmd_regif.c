/****************************************Copyright (c)**************************************************
**                                       �? �? �? �?
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**�?  �?  �? shell_cmd_regif.c
**�?  �?  �? 周江�?
**最后修改日�? 
**�?       �? shell命令的注册接�?
**              
**--------------History----------------------------------------------------------------------------
** 创建�? 周江�?
** �? �? v1.0
** 日　�? 2013.08.10
** 描　�? First version
**
**--------------Cureent version----------------------------------------------------------------------------
** 修改�? 
** 日　�? 
** 描　�? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_MODULE_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_SYSINFO_SUPPORT
COMMAND_DECLARE(sysinfo);
#endif
#if CMD_LIST_SUPPORT
COMMAND_DECLARE(list);
#endif
#if CMD_DETAIL_SUPPORT
COMMAND_DECLARE(detail);
#endif

#if CMD_STATI_SUPPORT
COMMAND_DECLARE(stati);
#endif
#if CMD_RESET_SUPPORT
COMMAND_DECLARE(reset);
#endif
#if CMD_ECHO_SUPPORT
COMMAND_DECLARE(echo);
#endif
#if CMD_MEM_SUPPORT
COMMAND_DECLARE(mem);
#endif
#if CMD_THREAD_SUPPORT
COMMAND_DECLARE(thread);
#endif
#if CMD_DATETIME_SUPPORT
COMMAND_DECLARE(datetime);
#endif

#if CMD_BLKDEV_SUPPORT
COMMAND_DECLARE(blkdev);
#endif

#if CMD_FS_SUPPORT
COMMAND_DECLARE(fs);
#endif
#if CMD_XMODEM_SUPPORT
COMMAND_DECLARE(xm);
#endif
#if CMD_DBGPOINT_SUPPORT
COMMAND_DECLARE(dbgp);
#endif
#if CMD_USER_SUPPORT
COMMAND_DECLARE(user);
#endif

#if CMD_BEEP_SUPPORT
COMMAND_DECLARE(beep);
#endif
#if CMD_LED_SUPPORT
COMMAND_DECLARE(led);
#endif
#if CMD_WATCHDOG_SUPPORT
COMMAND_DECLARE(wdog);
#endif
#if CMD_DIAGNOSE_SUPPORT
COMMAND_DECLARE(diagnose);
#endif
#if CMD_DAEMON_SUPPORT
COMMAND_DECLARE(daemon);
#endif
#if CMD_TIMER_SUPPORT
COMMAND_DECLARE(timer);
#endif
#if CMD_LOG_SUPPORT
COMMAND_DECLARE(log);
#endif

void _wind_register_all_cmd(void)
{
#if CMD_SYSINFO_SUPPORT
    wind_cmd_register(COMMAND(sysinfo));
#endif
#if CMD_LIST_SUPPORT
    wind_cmd_register(COMMAND(list));
#endif
#if CMD_DETAIL_SUPPORT
    wind_cmd_register(COMMAND(detail));
#endif
#if CMD_STATI_SUPPORT
    wind_cmd_register(COMMAND(stati));
#endif
#if CMD_RESET_SUPPORT
    wind_cmd_register(COMMAND(reset));
#endif
#if CMD_ECHO_SUPPORT
    wind_cmd_register(COMMAND(echo));
#endif
#if CMD_MEM_SUPPORT
    wind_cmd_register(COMMAND(mem));
#endif
#if CMD_THREAD_SUPPORT
    wind_cmd_register(COMMAND(thread));
#endif

#if CMD_DATETIME_SUPPORT
    wind_cmd_register(COMMAND(datetime));
#endif
#if CMD_BLKDEV_SUPPORT
    wind_cmd_register(COMMAND(blkdev));
#endif
#if CMD_FS_SUPPORT
    wind_cmd_register(COMMAND(fs));
#endif
#if CMD_XMODEM_SUPPORT
    wind_cmd_register(COMMAND(xm));
#endif
#if CMD_DBGPOINT_SUPPORT
    wind_cmd_register(COMMAND(dbgp));
#endif
#if CMD_USER_SUPPORT
    wind_cmd_register(COMMAND(user));
#endif

#if CMD_BEEP_SUPPORT
    wind_cmd_register(COMMAND(beep));
#endif
#if CMD_LED_SUPPORT
    wind_cmd_register(COMMAND(led));
#endif
#if CMD_WATCHDOG_SUPPORT
    wind_cmd_register(COMMAND(wdog));
#endif
#if CMD_DIAGNOSE_SUPPORT
    wind_cmd_register(COMMAND(diagnose));
#endif
#if CMD_DAEMON_SUPPORT
    wind_cmd_register(COMMAND(daemon));
#endif
#if CMD_TIMER_SUPPORT
    wind_cmd_register(COMMAND(timer));
#endif
#if CMD_LOG_SUPPORT
    wind_cmd_register(COMMAND(log));
#endif

}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
