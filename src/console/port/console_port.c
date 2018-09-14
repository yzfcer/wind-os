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
CMD_DECLARE(list);
CMD_DECLARE(stati);
CMD_DECLARE(reset);
CMD_DECLARE(echo);
CMD_DECLARE(mem);
CMD_DECLARE(thread);
#if WIND_DATETIME_SUPPORT
CMD_DECLARE(datetime);
#endif
CMD_DECLARE(test);
#if WIND_BLK_DRVFRAME_SUPPORT
CMD_DECLARE(blkdev);
#endif
#if WIND_LUA_SUPPORT
CMD_DECLARE(lua);
#endif
#if WIND_FS_SUPPORT
CMD_DECLARE(fs);
#endif
#if (WIND_XMODEM_SUPPORT && WIND_FS_SUPPORT)
CMD_DECLARE(xmodem);
#endif
#if WIND_DRVFRAME_SUPPORT
CMD_DECLARE(beep);
CMD_DECLARE(led);
#endif

void _wind_register_all_cmd(w_console_s *ctrl)
{
    wind_cmd_register(COMMAND(list),1);
    wind_cmd_register(COMMAND(stati),1);
    wind_cmd_register(COMMAND(reset),1);
    wind_cmd_register(COMMAND(echo),1);
    wind_cmd_register(COMMAND(mem),1);
    wind_cmd_register(COMMAND(thread),1);
    wind_cmd_register(COMMAND(test),1);
#if WIND_DATETIME_SUPPORT
    wind_cmd_register(COMMAND(datetime),1);
#endif
#if WIND_BLK_DRVFRAME_SUPPORT
    wind_cmd_register(COMMAND(blkdev),1);
#endif
#if WIND_LUA_SUPPORT
    wind_cmd_register(COMMAND(lua),1);
#endif
#if WIND_FS_SUPPORT
    wind_cmd_register(COMMAND(fs),1);
#endif
#if (WIND_XMODEM_SUPPORT && WIND_FS_SUPPORT)
    wind_cmd_register(COMMAND(xmodem),1);
#endif
#if WIND_DRVFRAME_SUPPORT
    wind_cmd_register(COMMAND(beep),1);
    wind_cmd_register(COMMAND(led),1);
#endif

}


#endif
