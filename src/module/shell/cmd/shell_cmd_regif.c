/****************************************Copyright (c)**************************************************
**                                       Ê∏? È£? Êµ? Â≤?
**
**                                       yzfcer@163.com
**
**--------------Êñá‰ª∂‰ø°ÊÅØ--------------------------------------------------------------------------------
**Êñ?  ‰ª?  Âê? shell_cmd_regif.c
**Âà?  Âª?  ‰∫? Âë®Ê±üÊù?
**ÊúÄÂêé‰øÆÊîπÊó•Êú? 
**Êè?       Ëø? shellÂëΩ‰ª§ÁöÑÊ≥®ÂÜåÊé•Âè?
**              
**--------------ÂéÜÂè≤ÁâàÊú¨‰ø°ÊÅØ----------------------------------------------------------------------------
** ÂàõÂª∫‰∫? Âë®Ê±üÊù?
** Áâ? Êú? v1.0
** Êó•„ÄÄÊú? 2013.08.10
** Êèè„ÄÄËø? ÂéüÂßãÁâàÊú¨
**
**--------------ÂΩìÂâçÁâàÊú¨‰øÆËÆ¢----------------------------------------------------------------------------
** ‰øÆÊîπ‰∫? 
** Êó•„ÄÄÊú? 
** Êèè„ÄÄËø? 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_SYSINFO_SUPPORT
COMMAND_DECLARE(sysinfo);
#endif
#if CMD_LIST_SUPPORT
COMMAND_DECLARE(list);
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
#if CMD_CUTEST_SUPPORT
COMMAND_DECLARE(cutest);
#endif
#if CMD_BLKDEV_SUPPORT
COMMAND_DECLARE(blkdev);
#endif
#if CMD_LUA_SUPPORT
COMMAND_DECLARE(lua);
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
#if CMD_PACK_SUPPORT
COMMAND_DECLARE(pack);
#endif
#if CMD_RCP_SUPPORT
COMMAND_DECLARE(rcp);
#endif


void _wind_register_all_cmd(void)
{
#if CMD_SYSINFO_SUPPORT
    wind_cmd_register(COMMAND(sysinfo),1);
#endif
#if CMD_LIST_SUPPORT
    wind_cmd_register(COMMAND(list),1);
#endif
#if CMD_STATI_SUPPORT
    wind_cmd_register(COMMAND(stati),1);
#endif
#if CMD_RESET_SUPPORT
    wind_cmd_register(COMMAND(reset),1);
#endif
#if CMD_ECHO_SUPPORT
    wind_cmd_register(COMMAND(echo),1);
#endif
#if CMD_MEM_SUPPORT
    wind_cmd_register(COMMAND(mem),1);
#endif
#if CMD_THREAD_SUPPORT
    wind_cmd_register(COMMAND(thread),1);
#endif
#if CMD_CUTEST_SUPPORT
    wind_cmd_register(COMMAND(cutest),1);
#endif
#if CMD_DATETIME_SUPPORT
    wind_cmd_register(COMMAND(datetime),1);
#endif
#if CMD_BLKDEV_SUPPORT
    wind_cmd_register(COMMAND(blkdev),1);
#endif
#if CMD_LUA_SUPPORT
    wind_cmd_register(COMMAND(lua),1);
#endif
#if CMD_FS_SUPPORT
    wind_cmd_register(COMMAND(fs),1);
#endif
#if CMD_XMODEM_SUPPORT
    wind_cmd_register(COMMAND(xm),1);
#endif
#if CMD_DBGPOINT_SUPPORT
    wind_cmd_register(COMMAND(dbgp),1);
#endif
#if CMD_USER_SUPPORT
    wind_cmd_register(COMMAND(user),1);
#endif

#if CMD_BEEP_SUPPORT
    wind_cmd_register(COMMAND(beep),1);
#endif
#if CMD_LED_SUPPORT
    wind_cmd_register(COMMAND(led),1);
#endif
#if CMD_WATCHDOG_SUPPORT
    wind_cmd_register(COMMAND(wdog),1);
#endif
#if CMD_DIAGNOSE_SUPPORT
    wind_cmd_register(COMMAND(diagnose),1);
#endif
#if CMD_DAEMON_SUPPORT
    wind_cmd_register(COMMAND(daemon),1);
#endif
#if CMD_TIMER_SUPPORT
    wind_cmd_register(COMMAND(timer),1);
#endif
#if CMD_PACK_SUPPORT
    wind_cmd_register(COMMAND(pack),1);
#endif
#if CMD_RCP_SUPPORT
    wind_cmd_register(COMMAND(rcp),1);
#endif

}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
