/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: console_port.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 控制台输出移植接口文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.08.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_LIST_SUPPORT
CMD_DECLARE(list);
#endif
#if CMD_STATI_SUPPORT
CMD_DECLARE(stati);
#endif
#if CMD_RESET_SUPPORT
CMD_DECLARE(reset);
#endif
#if CMD_ECHO_SUPPORT
CMD_DECLARE(echo);
#endif
#if CMD_MEM_SUPPORT
CMD_DECLARE(mem);
#endif
#if CMD_THREAD_SUPPORT
CMD_DECLARE(thread);
#endif
#if CMD_DATETIME_SUPPORT
CMD_DECLARE(datetime);
#endif
#if CMD_TEST_SUPPORT
CMD_DECLARE(test);
#endif
#if CMD_BLKDEV_SUPPORT
CMD_DECLARE(blkdev);
#endif
#if CMD_LUA_SUPPORT
CMD_DECLARE(lua);
#endif
#if CMD_FS_SUPPORT
CMD_DECLARE(fs);
#endif
#if CMD_XMODEM_SUPPORT
CMD_DECLARE(xmodem);
#endif
#if CMD_BEEP_SUPPORT
CMD_DECLARE(beep);
#endif
#if CMD_LED_SUPPORT
CMD_DECLARE(led);
#endif

void _wind_register_all_cmd(w_console_s *ctrl)
{
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
#if CMD_TEST_SUPPORT
    wind_cmd_register(COMMAND(test),1);
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
    wind_cmd_register(COMMAND(xmodem),1);
#endif
#if CMD_BEEP_SUPPORT
    wind_cmd_register(COMMAND(beep),1);
#endif
#if CMD_LED_SUPPORT
    wind_cmd_register(COMMAND(led),1);
#endif

}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
